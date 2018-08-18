#ifndef SCIQLOP_IDATAPROVIDER_H
#define SCIQLOP_IDATAPROVIDER_H

#include "CoreGlobal.h"
#include <Common/deprecate.h>

#include <memory>

#include <QObject>
#include <QUuid>

#include <Common/MetaTypes.h>

#include <Data/DateTimeRange.h>

#include <functional>

class DataProviderParameters;
class IDataSeries;
DEPRECATE(
    class QNetworkReply;
    class QNetworkRequest;
)

/**
 * @brief The IDataProvider interface aims to declare a data provider.
 *
 * A data provider is an entity that generates data and returns it according to various parameters
 * (time interval, product to retrieve the data, etc.)
 * Since its client mihgt use it from different threads it has to be either stateless  and/or thread safe
 *
 * @sa IDataSeries
 */
class SCIQLOP_CORE_EXPORT IDataProvider : public QObject {

    Q_OBJECT
public:
    virtual ~IDataProvider() noexcept = default;
    virtual std::shared_ptr<IDataProvider> clone() const = 0;

    // Synchronous call -> asyncGetData may be written for asynchronous get
    virtual IDataSeries* getData(const DataProviderParameters &parameters) = 0;

signals:

    void progress(QUuid requestID, double progress);

};

// Required for using shared_ptr in signals/slots
SCIQLOP_REGISTER_META_TYPE(IDATAPROVIDER_PTR_REGISTRY, std::shared_ptr<IDataProvider>)
SCIQLOP_REGISTER_META_TYPE(IDATAPROVIDER_FUNCTION_REGISTRY,
                           std::function<void(QNetworkReply *, QUuid)>)

#endif // SCIQLOP_IDATAPROVIDER_H
