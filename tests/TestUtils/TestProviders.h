#ifndef TESTPROVIDER_H
#define TESTPROVIDER_H

#include <Data/DataProviderParameters.h>
#include <Data/IDataProvider.h>
#include <Data/ScalarTimeSerie.h>
#include <QUuid>
#include <QtGlobal>
#include <QtTest>
#include <TimeSeries.h>
#include <Variable/Variable2.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <numeric>

template<int slope> class SimpleRange : public IDataProvider
{
public:
  SimpleRange() = default;

  int callCounter = 0;
  std::shared_ptr<IDataProvider> clone() const override
  {
    return std::make_shared<SimpleRange>();
  }

  TimeSeries::ITimeSerie*
  getData(const DataProviderParameters& parameters) override
  {
    callCounter += 1;
    std::size_t size =
        static_cast<std::size_t>(floor(parameters.m_Range.m_TEnd) -
                                 ceil(parameters.m_Range.m_TStart) + 1.);
    auto serie = new ScalarTimeSerie(size);
    std::generate(std::begin(*serie), std::end(*serie),
                  [i = ceil(parameters.m_Range.m_TStart)]() mutable {
                    return std::pair<double, double>{i, i++ * slope};
                  });
    return serie;
  }
};

template<class T> auto sumdiff(T begin, T end)
{
  std::vector<double> diff_vect(end - begin - 1);
  auto diff = [](auto next, auto item) { return next.value() - item.value(); };
  std::transform(begin + 1, end, begin, diff_vect.begin(), diff);
  return std::accumulate(diff_vect.cbegin(), diff_vect.cend(), 0);
}

template<int slope = 1> struct RangeType
{
  static void check_properties(std::shared_ptr<Variable2> v, DateTimeRange r)
  {
    // TODO
  }
};

template<class T>
void check_variable_state(std::shared_ptr<Variable2> v, DateTimeRange r)
{
  auto range = v->data()->axis_range(0);
  QVERIFY(range.first >= r.m_TStart);
  QVERIFY(range.second <= r.m_TEnd);
  T::check_properties(v, r);
}

#endif
