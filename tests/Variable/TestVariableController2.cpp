#include <cmath>
#include <QtTest>
#include <QObject>
#include <Variable/VariableController2.h>
#include <Data/DateTimeRange.h>
#include <Data/IDataProvider.h>
#include <Data/ScalarSeries.h>
#include <Data/DataProviderParameters.h>
#include <Common/containers.h>

class FakeProvider: public IDataProvider
{
public:
    FakeProvider() = default;

    std::shared_ptr<IDataProvider> clone() const override{ return std::make_shared<FakeProvider>(); }

    IDataSeries* getData(const DataProviderParameters &parameters) override
    {
        auto tstart = parameters.m_Times[0].m_TStart;
        auto tend = parameters.m_Times[0].m_TEnd;
        std::vector<double> x;
        std::vector<double> y;
        for(auto i = tstart;i<tend;i+=1.) //1 seconde data resolution
        {
            x.push_back(i);
            y.push_back(i);
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
        auto provider = std::make_shared<FakeProvider>();
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
        auto provider = std::make_shared<FakeProvider>();
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
        auto provider = std::make_shared<FakeProvider>();
        auto range1 = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(14,00),
                                                  QDate(2018,8,7),QTime(16,00));
        auto range2 = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(12,00),
                                                  QDate(2018,8,7),QTime(18,00));
        auto var1 = vc.createVariable("var1", {}, provider, range1);
        QCOMPARE(var1->nbPoints(), int(range1.delta()));
        vc.changeRange(var1, range2);
        QCOMPARE(var1->nbPoints(), int(range2.delta()));

    }

private:

};


QTEST_MAIN(TestVariableController2)

#include "TestVariableController2.moc"

