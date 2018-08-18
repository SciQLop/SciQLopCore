#ifndef TESTPROVIDER_H
#define TESTPROVIDER_H

#include <memory>
#include <cmath>
#include <algorithm>
#include <numeric>

#include <QUuid>
#include <QtGlobal>
#include <QtTest>

#include <Data/IDataProvider.h>
#include <Data/DataProviderParameters.h>
#include <Data/DataSeries.h>
#include <Data/ScalarSeries.h>
#include <Variable/Variable.h>


template<int slope>
class SimpleRange: public IDataProvider
{
public:
    SimpleRange() = default;

    int callCounter = 0;
    std::shared_ptr<IDataProvider> clone() const override{ return std::make_shared<SimpleRange>(); }

    IDataSeries* getData(const DataProviderParameters &parameters) override
    {
        callCounter+=1;
        auto tstart = parameters.m_Times[0].m_TStart;
        auto tend = parameters.m_Times[0].m_TEnd;
        std::vector<double> x;
        std::vector<double> y;
        for(double i = ceil(tstart);i<=floor(tend);i+=1.) //1 seconde data resolution
        {
            x.push_back(i);
            y.push_back(i*slope);
        }
        auto serie = new ScalarSeries(std::move(x),std::move(y),Unit("Secondes",true),Unit("Volts",false));
        return serie;
    }

};


template <class T>
auto sumdiff(T begin, T end)
{
    std::vector<double> diff_vect(end-begin-1);
    auto diff = [](auto next,auto item)
    {
        return  next.value() - item.value();
    };
    std::transform (begin+1, end, begin, diff_vect.begin(),diff);
    return  std::accumulate(diff_vect.cbegin(), diff_vect.cend(), 0);
}

template <int slope=1>
struct RangeType
{
    static void check_properties(std::shared_ptr<Variable> v, DateTimeRange r)
    {
        auto bounds = v->dataSeries()->valuesBounds(r.m_TStart, r.m_TEnd);
        auto s = sumdiff(bounds.first, bounds.second) / slope;
        auto nbpoints = bounds.second - bounds.first+1.;
        QCOMPARE(nbpoints, int(s)+2);//<- @TODO weird has to be investigated why +2?
        QCOMPARE(bounds.first->x(), bounds.first->value()/slope);
    }
};

template <class T>
void check_variable_state(std::shared_ptr<Variable> v, DateTimeRange r)
{
    auto bounds = v->dataSeries()->valuesBounds(r.m_TStart, r.m_TEnd);
    //generated data has to be inside range
    QVERIFY(bounds.first->x() >= r.m_TStart);
    QVERIFY(bounds.second->x() <= r.m_TEnd);
    T::check_properties(v,r);
}

#endif
