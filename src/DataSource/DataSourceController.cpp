#include "DataSource/DataSourceController.h"
#include "DataSource/DataSourceItem.h"

#include <Data/IDataProvider.h>

#include <QMutex>
#include <QThread>

#include <QDataStream>
#include <QDir>
#include <QStandardPaths>

Q_LOGGING_CATEGORY(LOG_DataSourceController, "DataSourceController")

class DataSourceController::DataSourceControllerPrivate {
public:
    QMutex m_WorkingMutex;
    /// Data sources registered
    QHash<QUuid, QString> m_DataSources;
    /// Data sources structures
    std::map<QUuid, std::unique_ptr<DataSourceItem> > m_DataSourceItems;
    /// Data providers registered
    /// @remarks Data providers are stored as shared_ptr as they can be sent to a variable and
    /// continue to live without necessarily the data source controller
    std::map<QUuid, std::shared_ptr<IDataProvider> > m_DataProviders;

    // Search for the first datasource item matching the specified data
    DataSourceItem *findDataSourceItem(const QVariantHash &data)
    {
        DataSourceItem *sourceItem = nullptr;
        for (const auto &item : m_DataSourceItems) {
            sourceItem = item.second->findItem(data, true);
            if (sourceItem) {
                break;
            }
        }

        return sourceItem;
    }

    // Search for the first datasource item matching the specified ID_DATA_KEY
    DataSourceItem *findDataSourceItem(const QString &datasourceIdKey)
    {
        DataSourceItem *sourceItem = nullptr;
        for (const auto &item : m_DataSourceItems) {
            sourceItem = item.second->findItem(datasourceIdKey, true);
            if (sourceItem) {
                break;
            }
        }

        return sourceItem;
    }
};

DataSourceController::DataSourceController(QObject *parent)
        : impl{spimpl::make_unique_impl<DataSourceControllerPrivate>()}
{
    qCDebug(LOG_DataSourceController()) << tr("DataSourceController construction")
                                        << QThread::currentThread();
}

DataSourceController::~DataSourceController()
{
    qCDebug(LOG_DataSourceController()) << tr("DataSourceController destruction")
                                        << QThread::currentThread();
    this->waitForFinish();
}

QUuid DataSourceController::registerDataSource(const QString &dataSourceName) noexcept
{
    auto dataSourceUid = QUuid::createUuid();
    impl->m_DataSources.insert(dataSourceUid, dataSourceName);

    return dataSourceUid;
}

void DataSourceController::setDataSourceItem(
    const QUuid &dataSourceUid, std::unique_ptr<DataSourceItem> dataSourceItem) noexcept
{
    if (!dataSourceItem) {
        qCWarning(LOG_DataSourceController())
            << tr("Data source item can't be registered (null item)");
        return;
    }

    if (impl->m_DataSources.contains(dataSourceUid)) {
        // The data provider is implicitly converted to a shared_ptr
        impl->m_DataSourceItems.insert(std::make_pair(dataSourceUid, std::move(dataSourceItem)));

        // Retrieves the data source item to emit the signal with it
        auto it = impl->m_DataSourceItems.find(dataSourceUid);
        if (it != impl->m_DataSourceItems.end()) {
            emit dataSourceItemSet(it->second.get());
        }
    }
    else {
        qCWarning(LOG_DataSourceController()) << tr("Can't set data source item for uid %1 : no "
                                                    "data source has been registered with the uid")
                                                     .arg(dataSourceUid.toString());
    }
}

void DataSourceController::setDataProvider(const QUuid &dataSourceUid,
                                           std::unique_ptr<IDataProvider> dataProvider) noexcept
{
    if (impl->m_DataSources.contains(dataSourceUid)) {
        impl->m_DataProviders.insert(std::make_pair(dataSourceUid, std::move(dataProvider)));
    }
    else {
        qCWarning(LOG_DataSourceController()) << tr("Can't set data provider for uid %1 : no data "
                                                    "source has been registered with the uid")
                                                     .arg(dataSourceUid.toString());
    }
}

void DataSourceController::loadProductItem(const QUuid &dataSourceUid,
                                           const DataSourceItem &productItem) noexcept
{
    if (productItem.type() == DataSourceItemType::PRODUCT
        || productItem.type() == DataSourceItemType::COMPONENT) {
        /// Retrieves the data provider of the data source (if any)
        auto it = impl->m_DataProviders.find(dataSourceUid);
        auto dataProvider = (it != impl->m_DataProviders.end()) ? it->second : nullptr;

        emit createVariable(productItem.name(), productItem.data(), dataProvider);
    }
    else {
        qCWarning(LOG_DataSourceController()) << tr("Can't load an item that is not a product");
    }
}

QByteArray DataSourceController::mimeDataForProductsData(const QVariantList &productsData)
{
    QByteArray encodedData;
    QDataStream stream{&encodedData, QIODevice::WriteOnly};

    stream << productsData;

    return encodedData;
}

QVariantList DataSourceController::productsDataForMimeData(const QByteArray &mimeData)
{
    QDataStream stream{mimeData};

    QVariantList productList;
    stream >> productList;

    return productList;
}

void DataSourceController::initialize()
{
    qCDebug(LOG_DataSourceController()) << tr("DataSourceController init")
                                        << QThread::currentThread();
    impl->m_WorkingMutex.lock();
    qCDebug(LOG_DataSourceController()) << tr("DataSourceController init END");
}

void DataSourceController::finalize()
{
    impl->m_WorkingMutex.unlock();
}

void DataSourceController::requestVariableFromProductIdKey(const QString &datasourceIdKey)
{
    auto sourceItem = impl->findDataSourceItem(datasourceIdKey);

    if (sourceItem) {
        auto sourceName = sourceItem->rootItem().name();
        auto sourceId = impl->m_DataSources.key(sourceName);
        loadProductItem(sourceId, *sourceItem);
    }
    else {
        qCWarning(LOG_DataSourceController()) << tr("requestVariable, product data not found");
    }
}

void DataSourceController::requestVariable(const QVariantHash &productData)
{
    auto sourceItem = impl->findDataSourceItem(productData);

    if (sourceItem) {
        auto sourceName = sourceItem->rootItem().name();
        auto sourceId = impl->m_DataSources.key(sourceName);
        loadProductItem(sourceId, *sourceItem);
    }
    else {
        qCWarning(LOG_DataSourceController()) << tr("requestVariable, product data not found");
    }
}

void DataSourceController::waitForFinish()
{
    QMutexLocker locker{&impl->m_WorkingMutex};
}
