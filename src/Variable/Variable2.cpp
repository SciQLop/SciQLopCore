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
  type Variable2::getter() const noexcept { return impl->getter(); }           \
  void Variable2::setter(const type& value) noexcept                           \
  {                                                                            \
    impl->setter(value);                                                       \
    emit updated(this->ID());                                                  \
  }

struct Variable2::VariablePrivate
{
  VariablePrivate(const QString& name, const QVariantHash& metadata)
      : m_Name{name}, m_Range{INVALID_RANGE}, m_Metadata{metadata}, m_TimeSerie{
                                                                        nullptr}
  {}
  VariablePrivate(const VariablePrivate& other) {}
  std::size_t nbPoints()
  {
    if(m_TimeSerie) return m_TimeSerie->base()->size();
    return 0;
  }
  DataSeriesType type() const
  {
    if(m_TimeSerie) return DataSeriesType(m_TimeSerie->index());
    return DataSeriesType::NONE;
  }

  PROPERTY_(m_Name, name, setName, QString)
  PROPERTY_(m_Range, range, setRange, DateTimeRange)
  PROPERTY_(m_Metadata, metadata, setMetadata, QVariantHash)
  AnyTimeSerie& dataSeries() { return *m_TimeSerie.get(); }
  void setDataSeries(std::unique_ptr<AnyTimeSerie>&& timeSerie)
  {
    QWriteLocker lock{&m_Lock};
    m_TimeSerie = std::move(timeSerie);
  }
  std::unique_ptr<AnyTimeSerie> m_TimeSerie;
  QReadWriteLock m_Lock;
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

DateTimeRange Variable2::range() const noexcept { return impl->range(); }

std::size_t Variable2::nbPoints() const noexcept { return impl->nbPoints(); }

AnyTimeSerie* Variable2::data() const noexcept {}

DataSeriesType Variable2::type() const noexcept { return impl->type(); }

QVariantHash Variable2::metadata() const noexcept {}

template<typename T>
void _merge(const std::vector<AnyTimeSerie*>& source, AnyTimeSerie* dest)
{
  //  std::for_each(
  //      std::cbegin(source) + 1, std::cend(source), [dest](AnyTimeSerie*
  //      serie) {
  //        std::copy(std::begin(serie->get<T>()), std::end(serie->get<T>()),
  //                  std::back_inserter(dest->get<T>()));
  //      });
}

std::unique_ptr<AnyTimeSerie>
merge(const std::vector<AnyTimeSerie*>& dataSeries)
{
  std::unique_ptr<AnyTimeSerie> ts;
  *ts = *dataSeries.front();
  switch(DataSeriesType(ts->index()))
  {
    case DataSeriesType::NONE: break;
    case DataSeriesType::SCALAR:
      _merge<ScalarTimeSerie>(dataSeries, ts.get());
      break;
    case DataSeriesType::VECTOR:
      _merge<VectorTimeSerie>(dataSeries, ts.get());
      break;
    case DataSeriesType::SPECTROGRAM:
      // merge<Spe>(dataSeries, ts.get());
      break;
  }
  return ts;
}

void Variable2::setData(const std::vector<AnyTimeSerie*>& dataSeries,
                        const DateTimeRange& range, bool notify)
{
  if(dataSeries.size())
  {
    impl->setDataSeries(merge(dataSeries));
    impl->setRange(range);
    if(notify) emit this->updated(this->ID());
  }
}
