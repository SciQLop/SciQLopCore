#pragma once

#include "SciQLopCore/Common/MetaTypes.h"
#include "SciQLopCore/Data/DateTimeRange.h"
#include <QObject>
#include <QUuid>
#include <TimeSeries.h>
#include <functional>
#include <memory>

class DataProviderParameters;
class IDataSeries;

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
class IDataProvider : public QObject
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


