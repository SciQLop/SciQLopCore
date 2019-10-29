#ifndef SCIQLOP_IDATAPROVIDER_H
#define SCIQLOP_IDATAPROVIDER_H

#include "CoreGlobal.h"

#include <Common/MetaTypes.h>
#include <deprecate.h>
#include <Data/DateTimeRange.h>
#include <QObject>
#include <QUuid>
#include <TimeSeries.h>
#include <functional>
#include <memory>

class DataProviderParameters;
class IDataSeries;
DEPRECATE(class QNetworkReply; class QNetworkRequest;)

/**
 * @brief The IDataProvider interface aims to declare a data provider.
 *
 * A data provider is an entity that generates data and returns it according to
 * various parameters (time interval, product to retrieve the data, etc.) Since
 * its client mihgt use it from different threads it has to be either stateless
 * and/or thread safe
 *
 * @sa IDataSeries
 */
class SCIQLOP_CORE_EXPORT IDataProvider : public QObject
{
  Q_OBJECT

  QUuid _id=QUuid::createUuid();

public:
  virtual ~IDataProvider() noexcept = default;

  virtual TimeSeries::ITimeSerie*
  getData(const DataProviderParameters& parameters) = 0;

  QUuid id() const { return _id; }
  QString name()
  {
    return QString("%1-%2").arg(this->metaObject()->className()).arg(id().toString());
  }
  
  operator QUuid(){return _id;}

signals:

  void progress(QUuid requestID, double progress);
};

// Required for using shared_ptr in signals/slots
SCIQLOP_REGISTER_META_TYPE(IDATAPROVIDER_PTR_REGISTRY,
                           std::shared_ptr<IDataProvider>)

#endif // SCIQLOP_IDATAPROVIDER_H
