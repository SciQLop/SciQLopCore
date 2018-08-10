#include <cmath>
#include <algorithm>
#include <numeric>
#include <QtTest>
#include <QObject>
#include <Variable/VariableController2.h>
#include <Data/DateTimeRange.h>
#include <Data/IDataProvider.h>
#include <Data/ScalarSeries.h>
#include <Data/DataProviderParameters.h>
#include <Common/containers.h>

template<int slope>
class SimpleRange: public IDataProvider
{
public:
    SimpleRange() = default;

    std::shared_ptr<IDataProvider> clone() const override{ return std::make_shared<SimpleRange>(); }

    IDataSeries* getData(const DataProviderParameters &parameters) override
    {
        auto tstart = parameters.m_Times[0].m_TStart;
        auto tend = parameters.m_Times[0].m_TEnd;
        std::vector<double> x;
        std::vector<double> y;
        for(auto i = tstart;i<tend;i+=1.) //1 seconde data resolution
        {
            x.push_back(i);
            y.push_back(i*slope);
        }
        auto serie = new ScalarSeries(std::move(x),std::move(y),Unit("Secondes",true),Unit("Volts",false));
        return serie;
    }



    void requestDataLoading(QUuid acqIdentifier, const DataProviderParameters &parameters) override
    {
        Q_UNUSED(acqIdentifier)
        Q_UNUSED(parameters)
    }

    void requestDataAborting(QUuid acqIdentifier) override
    {
        Q_UNUSED(acqIdentifier)
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
        QCOMPARE(r.m_TStart, bounds.first->value()/slope);
    }
};

template <class T>
void check_variable_state(std::shared_ptr<Variable> v, DateTimeRange r)
{
    auto bounds = v->dataSeries()->valuesBounds(r.m_TStart, r.m_TEnd);
    auto nbpoints = bounds.second - bounds.first+1.;
    QCOMPARE(nbpoints, int(static_cast<double>(r.delta())));
    T::check_properties(v,r);
}

class TestVariableController2 : public QObject
{
    Q_OBJECT
public:
    explicit TestVariableController2(QObject *parent = nullptr) : QObject(parent){}
signals:

private slots:
    void initTestCase(){}
    void cleanupTestCase(){}

    void testCreateVariable()
    {
        VariableController2 vc;
        bool callbackCalled = false;
        connect(&vc,&VariableController2::variableAdded, [&callbackCalled](std::shared_ptr<Variable>){callbackCalled=true;});
        auto provider = std::make_shared<SimpleRange<1>>();
        QVERIFY(!callbackCalled);
        auto var1 = vc.createVariable("var1",{},provider,DateTimeRange());
        QVERIFY(SciQLop::containers::contains(vc.variables(), var1));
        QVERIFY(callbackCalled);
    }

    void testDeleteVariable()
    {
        VariableController2 vc;
        bool callbackCalled = false;
        connect(&vc,&VariableController2::variableDeleted, [&callbackCalled](std::shared_ptr<Variable>){callbackCalled=true;});
        auto provider = std::make_shared<SimpleRange<1>>();
        auto var1 = vc.createVariable("var1",{},provider,DateTimeRange());
        QVERIFY(SciQLop::containers::contains(vc.variables(), var1));
        QVERIFY(!callbackCalled);
        vc.deleteVariable(var1);
        QVERIFY(!SciQLop::containers::contains(vc.variables(), var1));
        QVERIFY(callbackCalled);
    }

    void testGetData()
    {
        VariableController2 vc;
        auto provider = std::make_shared<SimpleRange<10>>();
        auto range = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(14,00),
                                                  QDate(2018,8,7),QTime(16,00));

        auto var1 = vc.createVariable("var1", {}, provider, range);
        check_variable_state<RangeType<10>>(var1, range);
    }

    void testZoomOut()
    {
        VariableController2 vc;
        auto provider = std::make_shared<SimpleRange<10>>();
        auto range = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(14,00),
                                                  QDate(2018,8,7),QTime(16,00));

        auto var1 = vc.createVariable("var1", {}, provider, range);
        check_variable_state<RangeType<10>>(var1, range);

        range *=2.;
        vc.changeRange(var1, range);
        check_variable_state<RangeType<10>>(var1, range);
    }

    void testPanRight()
    {
        VariableController2 vc;
        auto provider = std::make_shared<SimpleRange<10>>();
        auto range = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(14,00),
                                                  QDate(2018,8,7),QTime(16,00));

        auto var1 = vc.createVariable("var1", {}, provider, range);
        check_variable_state<RangeType<10>>(var1, range);

        range += Seconds<double>{1000.};
        vc.changeRange(var1, range);
        check_variable_state<RangeType<10>>(var1, range);
    }

};


QTEST_MAIN(TestVariableController2)

#include "TestVariableController2.moc"

