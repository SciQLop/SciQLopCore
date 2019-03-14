#include "Variable/Variable.h"

#include <Common/debug.h>
#include <Data/DateTimeRange.h>
#include <Data/IDataSeries.h>
#include <QMutex>
#include <QReadWriteLock>
#include <QThread>
#include <optional>

Q_LOGGING_CATEGORY(LOG_Variable, "Variable")

/**
 * Searches in metadata for a value that can be converted to DataSeriesType
 * @param metadata the metadata where to search
 * @return the value converted to a DataSeriesType if it was found, UNKNOWN type
 * otherwise
 * @sa DataSeriesType
 */
static DataSeriesType findDataSeriesType(const QVariantHash& metadata)
{
  auto dataSeriesType = DataSeriesType::NONE;

  // Go through the metadata and stop at the first value that could be converted
  // to DataSeriesType
  for(auto it = metadata.cbegin(), end = metadata.cend();
      it != end && dataSeriesType == DataSeriesType::NONE; ++it)
  {
    dataSeriesType = DataSeriesTypeUtils::fromString(it.value().toString());
  }

  return dataSeriesType;
}

#define VP_PROPERTY(property, getter, setter, type)                            \
  type getter() noexcept                                                       \
  {                                                                            \
    QReadLocker lock{&m_Lock};                                                 \
    return property;                                                           \
  }                                                                            \
  void setter(const type& getter) noexcept                                     \
  {                                                                            \
    QWriteLocker lock{&m_Lock};                                                \
    property = getter;                                                         \
  }                                                                            \
  type property;

#define V_FW_GETTER_SETTER(getter, setter, type)                               \
  type Variable::getter() const noexcept { return impl->getter(); }            \
  void Variable::setter(const type& getter) noexcept                           \
  {                                                                            \
    impl->setter(getter);                                                      \
    emit updated(this->ID());                                                  \
  }

struct Variable::VariablePrivate
{
  explicit VariablePrivate(const QString& name, const QVariantHash& metadata)
      : m_Name{name}, m_Range{INVALID_RANGE}, m_CacheRange{INVALID_RANGE},
        m_Metadata{metadata}, m_DataSeries{nullptr}, m_RealRange{INVALID_RANGE},
        m_NbPoints{0}, m_Type{findDataSeriesType(m_Metadata)}
  {}

  VariablePrivate(const VariablePrivate& other)
      : m_Name{other.m_Name}, m_Range{other.m_Range},
        m_CacheRange{other.m_CacheRange}, m_Metadata{other.m_Metadata},
        m_DataSeries{other.m_DataSeries != nullptr ? other.m_DataSeries->clone()
                                                   : nullptr},
        m_RealRange{other.m_RealRange},
        m_NbPoints{other.m_NbPoints}, m_Type{findDataSeriesType(m_Metadata)}
  {}

  void lockRead() { m_Lock.lockForRead(); }
  void lockWrite() { m_Lock.lockForWrite(); }
  void unlock() { m_Lock.unlock(); }

  void purgeDataSeries()
  {
    if(m_DataSeries)
    { m_DataSeries->purge(m_CacheRange.m_TStart, m_CacheRange.m_TEnd); }
    updateRealRange();
    updateNbPoints();
  }
  void mergeDataSeries(const std::vector<IDataSeries*>& dataseries,
                       bool overwrite = false)
  {
    QWriteLocker lock{&m_Lock};
    for(auto ds : dataseries)
    {
      if(!overwrite & bool(m_DataSeries))
        m_DataSeries->merge(ds);
      else
        m_DataSeries = ds->clone();
    }
  }
  void updateNbPoints()
  {
    m_NbPoints = m_DataSeries ? m_DataSeries->nbPoints() : 0;
  }

  /// Updates real range according to current variable range and data series
  void updateRealRange()
  {
    if(m_DataSeries)
    {
      auto lock       = m_DataSeries->getReadLock();
      auto end        = m_DataSeries->cend();
      auto minXAxisIt = m_DataSeries->minXAxisData(m_Range.m_TStart);
      auto maxXAxisIt = m_DataSeries->maxXAxisData(m_Range.m_TEnd);
      if(minXAxisIt != end && maxXAxisIt != end &&
         minXAxisIt->x() <= maxXAxisIt->x())
        m_RealRange = DateTimeRange{minXAxisIt->x(), maxXAxisIt->x()};
      else
        m_RealRange = std::nullopt;
    }
    else
    {
      m_RealRange = std::nullopt;
    }
  }

  VP_PROPERTY(m_Name, name, setName, QString)
  VP_PROPERTY(m_Range, range, setRange, DateTimeRange)
  VP_PROPERTY(m_CacheRange, cacheRange, setCacheRange, DateTimeRange)
  VP_PROPERTY(m_Metadata, metadata, setMetadata, QVariantHash)
  VP_PROPERTY(m_DataSeries, dataSeries, setDataSeries,
              std::shared_ptr<IDataSeries>)
  VP_PROPERTY(m_RealRange, realRange, setRealRange,
              std::optional<DateTimeRange>)
  unsigned int m_NbPoints;
  VP_PROPERTY(m_Type, type, setType, DataSeriesType)
  QReadWriteLock m_Lock;
};

Variable::Variable(const QString& name, const QVariantHash& metadata)
    : impl{spimpl::make_unique_impl<VariablePrivate>(name, metadata)},
      _uuid{QUuid::createUuid()}
{}

Variable::Variable(const Variable& other)
    : impl{spimpl::make_unique_impl<VariablePrivate>(*other.impl)},
      _uuid{QUuid::createUuid()} // is a clone but must have a != uuid
{}

std::shared_ptr<Variable> Variable::clone() const
{
  return std::make_shared<Variable>(*this);
}

V_FW_GETTER_SETTER(name, setName, QString)

DateTimeRange Variable::range() const noexcept { return impl->range(); }

void Variable::setRange(const DateTimeRange& range, bool notify) noexcept
{
  impl->setRange(range);
  impl->updateRealRange();
  if(notify) emit this->updated(this->ID());
}

V_FW_GETTER_SETTER(cacheRange, setCacheRange, DateTimeRange)

unsigned int Variable::nbPoints() const noexcept { return impl->m_NbPoints; }

std::optional<DateTimeRange> Variable::realRange() const noexcept
{
  return impl->realRange();
}

void Variable::updateData(const std::vector<IDataSeries*>& dataSeries,
                          const DateTimeRange& newRange,
                          const DateTimeRange& newCacheRange, bool notify)
{
  {
    QWriteLocker lock{&m_lock};
    impl->mergeDataSeries(dataSeries);
    impl->setRange(newRange);
    impl->setCacheRange(newCacheRange);
    impl->purgeDataSeries();
  }
  if(notify) emit updated(this->ID());
}

void Variable::setData(const std::vector<IDataSeries*>& dataSeries,
                       const DateTimeRange& newRange,
                       const DateTimeRange& newCacheRange, bool notify)
{
  {
    QWriteLocker lock{&m_lock};
    impl->mergeDataSeries(dataSeries, true);
    impl->setRange(newRange);
    impl->setCacheRange(newCacheRange);
    impl->purgeDataSeries();
  }
  if(notify) emit updated(this->ID());
}

std::shared_ptr<IDataSeries> Variable::dataSeries() const noexcept
{
  return impl->dataSeries();
}

DataSeriesType Variable::type() const noexcept { return impl->type(); }

QVariantHash Variable::metadata() const noexcept
{
  impl->lockRead();
  auto metadata = impl->m_Metadata;
  impl->unlock();
  return metadata;
}
