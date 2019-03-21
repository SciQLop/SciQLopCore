#include "Variable/Variable2.h"

#define PROPERTY_(property, getter, setter, type)                              \
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
  type Variable2::getter()                                                     \
  {                                                                            \
    QReadLocker lock{&this->m_lock};                                           \
    return impl->getter();                                                     \
  }                                                                            \
  void Variable2::setter(const type& value)                                    \
  {                                                                            \
    {                                                                          \
      QWriteLocker lock{&this->m_lock};                                        \
      impl->setter(value);                                                     \
    }                                                                          \
    emit updated(this->ID());                                                  \
  }

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

std::shared_ptr<TimeSeries::ITimeSerie>
clone_ts(const std::shared_ptr<TimeSeries::ITimeSerie>& ts)
{
  if(auto scal_ts = std::dynamic_pointer_cast<ScalarTimeSerie>(ts))
    return std::make_shared<ScalarTimeSerie>(*scal_ts);
  if(auto scal_ts = std::dynamic_pointer_cast<VectorTimeSerie>(ts))
    return std::make_shared<VectorTimeSerie>(*scal_ts);
  if(auto scal_ts = std::dynamic_pointer_cast<SpectrogramTimeSerie>(ts))
    return std::make_shared<SpectrogramTimeSerie>(*scal_ts);
  return nullptr;
}

struct Variable2::VariablePrivate
{
  VariablePrivate(const QString& name, const QVariantHash& metadata)
      : m_Name{name}, m_Range{INVALID_RANGE}, m_Metadata{metadata}, m_TimeSerie{
                                                                        nullptr}
  {
    switch(findDataSeriesType(metadata))
    {
      case DataSeriesType::SCALAR:
        m_TimeSerie = std::make_shared<ScalarTimeSerie>(ScalarTimeSerie{});
        break;
      case DataSeriesType::VECTOR:
        m_TimeSerie = std::make_shared<VectorTimeSerie>(VectorTimeSerie{});
        break;
      case DataSeriesType::SPECTROGRAM:
        m_TimeSerie =
            std::make_shared<SpectrogramTimeSerie>(SpectrogramTimeSerie{});
        break;
      default: break;
    }
  }

  VariablePrivate(const VariablePrivate& other)
      : m_Name{other.m_Name}, m_Range{other.m_Range},
        m_Metadata{other.m_Metadata}, m_RealRange{other.m_RealRange}
  {
    m_TimeSerie = clone_ts(other.m_TimeSerie);
  }

  std::size_t nbPoints()
  {
    if(m_TimeSerie) return m_TimeSerie->size();
    return 0;
  }
  DataSeriesType type() const
  {
    return DataSeriesTypeUtils::type(m_TimeSerie.get());
  }

  PROPERTY_(m_Name, name, setName, QString)
  PROPERTY_(m_Range, range, setRange, DateTimeRange)
  PROPERTY_(m_Metadata, metadata, setMetadata, QVariantHash)
  PROPERTY_(m_RealRange, realRange, setRealRange, std::optional<DateTimeRange>)
  std::shared_ptr<TimeSeries::ITimeSerie> dataSeries() { return m_TimeSerie; }
  void setDataSeries(std::shared_ptr<TimeSeries::ITimeSerie>&& timeSerie)
  {
    QWriteLocker lock{&m_Lock};
    m_TimeSerie = timeSerie;
    if(m_TimeSerie) { setRealRange(DateTimeRange(m_TimeSerie->axis_range(0))); }
    else
    {
      setRealRange(std::nullopt);
    }
  }
  std::shared_ptr<TimeSeries::ITimeSerie> m_TimeSerie;
  QReadWriteLock m_Lock{QReadWriteLock::Recursive};
};

Variable2::Variable2(const QString& name, const QVariantHash& metadata)
    : impl{spimpl::make_unique_impl<VariablePrivate>(name, metadata)},
      _uuid{QUuid::createUuid()}
{}

Variable2::Variable2(const Variable2& other)
    : impl{spimpl::make_unique_impl<VariablePrivate>(*other.impl)},
      _uuid{QUuid::createUuid()} // is a clone but must have a != uuid
{}

std::shared_ptr<Variable2> Variable2::clone() const
{
  return std::make_shared<Variable2>(*this);
}

V_FW_GETTER_SETTER(name, setName, QString)

DateTimeRange Variable2::range() { return impl->range(); }

void Variable2::setRange(const DateTimeRange& range)
{
  QWriteLocker lock{&m_lock};
  impl->setRange(range);
}

std::optional<DateTimeRange> Variable2::realRange()
{
  QReadLocker lock{&m_lock};
  return impl->realRange();
}

std::size_t Variable2::nbPoints() { return impl->nbPoints(); }

std::shared_ptr<TimeSeries::ITimeSerie> Variable2::data()
{
  return impl->dataSeries();
}

DataSeriesType Variable2::type()
{
  QReadLocker lock{&m_lock};
  return impl->type();
}

QVariantHash Variable2::metadata() const noexcept { return impl->metadata(); }

template<typename T>
std::shared_ptr<TimeSeries::ITimeSerie>
_merge(std::vector<TimeSeries::ITimeSerie*> source, const DateTimeRange& range)
{
  std::sort(std::begin(source), std::end(source),
            [](TimeSeries::ITimeSerie* a, TimeSeries::ITimeSerie* b) {
              if(a->size() && b->size()) return a->t(0) < b->t(0);
              return false;
            });
  std::shared_ptr<TimeSeries::ITimeSerie> dest = std::make_shared<T>();
  std::for_each(
      std::begin(source), std::end(source),
      [&dest, &range](TimeSeries::ITimeSerie* serie) {
        auto& ts    = *static_cast<T*>(serie);
        auto last_t = range.m_TStart;
        if(dest->size()) last_t = dest->axis(0).back();

        std::copy(std::upper_bound(
                      std::begin(ts), std::end(ts), last_t,
                      [](const auto& a, const auto& b) { return a < b.t(); }),
                  std::lower_bound(
                      std::begin(ts), std::end(ts), range.m_TEnd,
                      [](const auto& a, const auto& b) { return a.t() < b; }),
                  std::back_inserter(*std::dynamic_pointer_cast<T>(dest)));
      });
  return dest;
}

std::shared_ptr<TimeSeries::ITimeSerie>
merge(const std::vector<TimeSeries::ITimeSerie*>& dataSeries,
      const DateTimeRange& range)
{
  if(dynamic_cast<ScalarTimeSerie*>(dataSeries.front()))
    return _merge<ScalarTimeSerie>(dataSeries, range);
  if(dynamic_cast<VectorTimeSerie*>(dataSeries.front()))
    return _merge<VectorTimeSerie>(dataSeries, range);
  if(dynamic_cast<SpectrogramTimeSerie*>(dataSeries.front()))
    return _merge<SpectrogramTimeSerie>(dataSeries, range);
  return std::shared_ptr<TimeSeries::ITimeSerie>{};
}

void Variable2::setData(const std::vector<TimeSeries::ITimeSerie*>& dataSeries,
                        const DateTimeRange& range, bool notify)
{
  if(dataSeries.size())
  {
    {
      QWriteLocker lock{&m_lock};
      impl->setDataSeries(merge(dataSeries, range));
      impl->setRange(range);
    }
    if(notify) emit this->updated(this->ID());
  }
}
