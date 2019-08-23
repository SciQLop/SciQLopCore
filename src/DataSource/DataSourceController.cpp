#include "DataSource/DataSourceController.h"

#include "DataSource/DataSourceItem.h"
#include "DataSource/DataSourceItemAction.h"

#include <Common/containers.h>
#include <Data/IDataProvider.h>
#include <QDataStream>
#include <QDir>
#include <QMutex>
#include <QStandardPaths>
#include <QThread>

Q_LOGGING_CATEGORY(LOG_DataSourceController, "DataSourceController")

std::unique_ptr<DataSourceItem> make_folder_item(const QString& name)
{
  return std::make_unique<DataSourceItem>(DataSourceItemType::NODE, name);
}

template<typename T>
DataSourceItem* make_path_items(const T& path_list_begin,
                                const T& path_list_end, DataSourceItem* root)
{
  std::for_each(path_list_begin, path_list_end,
                [&root](const auto& folder_name) mutable {
                  auto folder_ptr = root->findItem(folder_name);
                  if(folder_ptr == nullptr)
                  {
                    auto folder = make_folder_item(folder_name);
                    folder_ptr  = folder.get();
                    root->appendChild(std::move(folder));
                  }
                  root = folder_ptr;
                });
  return root;
}

std::unique_ptr<DataSourceItem>
make_product_item(const QVariantHash& metaData, const QUuid& dataSourceUid,
                  const QString& DATA_SOURCE_NAME, DataSourceController* dc)
{
  auto result =
      std::make_unique<DataSourceItem>(DataSourceItemType::PRODUCT, metaData);

  // Adds plugin name to product metadata
  result->setData(DataSourceItem::PLUGIN_DATA_KEY, DATA_SOURCE_NAME);
  result->setData(DataSourceItem::ID_DATA_KEY,
                  metaData.value(DataSourceItem::NAME_DATA_KEY));

  auto productName = metaData.value(DataSourceItem::NAME_DATA_KEY).toString();

  // Add action to load product from DataSourceController
  result->addAction(std::make_unique<DataSourceItemAction>(
      QObject::tr("Load %1 product").arg(productName),
      [productName, dataSourceUid, dc](DataSourceItem& item) {
        if(dc) { dc->loadProductItem(dataSourceUid, item); }
      }));

  return result;
}

class DataSourceController::DataSourceControllerPrivate
{
public:
  QMutex m_WorkingMutex;
  /// Data sources registered
  QHash<QUuid, QString> m_DataSources;
  /// Data sources structures
  std::map<QUuid, std::unique_ptr<DataSourceItem>> m_DataSourceItems;
  /// Data providers registered
  /// @remarks Data providers are stored as shared_ptr as they can be sent to a
  /// variable and continue to live without necessarily the data source
  /// controller
  std::map<QUuid, std::shared_ptr<IDataProvider>> m_DataProviders;

  // Search for the first datasource item matching the specified data
  DataSourceItem* findDataSourceItem(const QVariantHash& data)
  {
    DataSourceItem* sourceItem = nullptr;
    for(const auto& item : m_DataSourceItems)
    {
      sourceItem = item.second->findItem(data, true);
      if(sourceItem) { break; }
    }

    return sourceItem;
  }

  // Search for the first datasource item matching the specified ID_DATA_KEY
  DataSourceItem* findDataSourceItem(const QString& datasourceIdKey)
  {
    DataSourceItem* sourceItem = nullptr;
    for(const auto& item : m_DataSourceItems)
    {
      sourceItem = item.second->findItem(datasourceIdKey, true);
      if(sourceItem) { break; }
    }

    return sourceItem;
  }
};

DataSourceController::DataSourceController(QObject* parent)
    : impl{spimpl::make_unique_impl<DataSourceControllerPrivate>()}
{
  qCDebug(LOG_DataSourceController())
      << tr("DataSourceController construction") << QThread::currentThread();
}

DataSourceController::~DataSourceController()
{
  qCDebug(LOG_DataSourceController())
      << tr("DataSourceController destruction") << QThread::currentThread();
  this->waitForFinish();
}

QUuid DataSourceController::registerDataSource(
    const QString& dataSourceName) noexcept
{
  auto dataSourceUid = QUuid::createUuid();
  impl->m_DataSources.insert(dataSourceUid, dataSourceName);

  return dataSourceUid;
}

void DataSourceController::registerProvider(IDataProvider *provider) noexcept
{
  impl->m_DataSources.insert(provider->id(), provider->name());
  impl->m_DataProviders.insert({provider->id(), std::unique_ptr<IDataProvider>{provider}});
  // TODO rethink this, I don't get why we have to do this crap
  // why do we need a root node for each provider
  setDataSourceItem(provider->id(),make_folder_item(provider->name()));
}

void DataSourceController::setDataSourceItem(
    const QUuid& dataSourceUid,
    std::unique_ptr<DataSourceItem> dataSourceItem) noexcept
{
  if(!dataSourceItem)
  {
    qCWarning(LOG_DataSourceController())
        << tr("Data source item can't be registered (null item)");
    return;
  }

  if(impl->m_DataSources.contains(dataSourceUid))
  {
    // The data provider is implicitly converted to a shared_ptr
    impl->m_DataSourceItems.insert(
        std::make_pair(dataSourceUid, std::move(dataSourceItem)));

    // Retrieves the data source item to emit the signal with it
    auto it = impl->m_DataSourceItems.find(dataSourceUid);
    if(it != impl->m_DataSourceItems.end())
    { emit dataSourceItemSet(it->second.get()); }
  }
  else
  {
    qCWarning(LOG_DataSourceController())
        << tr("Can't set data source item for uid %1 : no "
              "data source has been registered with the uid")
               .arg(dataSourceUid.toString());
  }
}



void DataSourceController::setDataSourceItem(const QUuid& dataSourceUid, const QString& path,
    const QMap<QString, QString> &metaData) noexcept
{
  if(auto it = impl->m_DataSourceItems.find(dataSourceUid);
     it != impl->m_DataSourceItems.end())
  {
    auto path_list = path.split('/', QString::SkipEmptyParts);
    auto name      = *(std::cend(path_list) - 1);
    auto path_item = make_path_items(
        std::cbegin(path_list), std::cend(path_list) - 1, it->second.get());
    QVariantHash meta_data{{DataSourceItem::NAME_DATA_KEY, name}};
    for(auto& key : metaData.keys())
    {
      meta_data[key] = metaData[key];
    }
    path_item->appendChild(
        make_product_item(meta_data, dataSourceUid, "test", this));
    emit dataSourceItemSet(it->second.get());
  }
}

void DataSourceController::setDataProvider(
    const QUuid& dataSourceUid,
    std::unique_ptr<IDataProvider> dataProvider) noexcept
{
  if(impl->m_DataSources.contains(dataSourceUid))
  {
    impl->m_DataProviders.insert(
        std::make_pair(dataSourceUid, std::move(dataProvider)));
  }
  else
  {
    qCWarning(LOG_DataSourceController())
        << tr("Can't set data provider for uid %1 : no data "
              "source has been registered with the uid")
               .arg(dataSourceUid.toString());
  }
}

void DataSourceController::loadProductItem(
    const QUuid& dataSourceUid, const DataSourceItem& productItem) noexcept
{
  if(productItem.type() == DataSourceItemType::PRODUCT ||
     productItem.type() == DataSourceItemType::COMPONENT)
  {
    /// Retrieves the data provider of the data source (if any)
    auto it = impl->m_DataProviders.find(dataSourceUid);
    auto dataProvider =
        (it != impl->m_DataProviders.end()) ? it->second : nullptr;

    emit createVariable(productItem.name(), productItem.data(), dataProvider);
  }
  else
  {
    qCWarning(LOG_DataSourceController())
        << tr("Can't load an item that is not a product");
  }
}

QByteArray
DataSourceController::mimeDataForProductsData(const QVariantList& productsData)
{
  QByteArray encodedData;
  QDataStream stream{&encodedData, QIODevice::WriteOnly};

  stream << productsData;

  return encodedData;
}

QVariantList
DataSourceController::productsDataForMimeData(const QByteArray& mimeData)
{
  QDataStream stream{mimeData};

  QVariantList productList;
  stream >> productList;

  return productList;
}

void DataSourceController::initialize()
{
  qCDebug(LOG_DataSourceController())
      << tr("DataSourceController init") << QThread::currentThread();
  impl->m_WorkingMutex.lock();
  qCDebug(LOG_DataSourceController()) << tr("DataSourceController init END");
}

void DataSourceController::finalize() { impl->m_WorkingMutex.unlock(); }

void DataSourceController::requestVariableFromProductIdKey(
    const QString& datasourceIdKey)
{
  auto sourceItem = impl->findDataSourceItem(datasourceIdKey);

  if(sourceItem)
  {
    auto sourceName = sourceItem->rootItem().name();
    auto sourceId   = impl->m_DataSources.key(sourceName);
    loadProductItem(sourceId, *sourceItem);
  }
  else
  {
    qCWarning(LOG_DataSourceController())
        << tr("requestVariable, product data not found");
  }
}

void DataSourceController::requestVariable(const QVariantHash& productData)
{
  auto sourceItem = impl->findDataSourceItem(productData);

  if(sourceItem)
  {
    auto sourceName = sourceItem->rootItem().name();
    auto sourceId   = impl->m_DataSources.key(sourceName);
    loadProductItem(sourceId, *sourceItem);
  }
  else
  {
    qCWarning(LOG_DataSourceController())
        << tr("requestVariable, product data not found");
  }
}

void DataSourceController::waitForFinish()
{
  QMutexLocker locker{&impl->m_WorkingMutex};
}
