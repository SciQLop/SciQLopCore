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

#include <TestUtils/TestProviders.h>

#define TEST_VC2_FIXTURE(slope) \
    VariableController2 vc; \
    auto provider = std::make_shared<SimpleRange<slope>>();\

#define TEST_VC2_CREATE_DEFAULT_VAR(name)\
    auto range = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(14,00),\
                                          QDate(2018,8,7),QTime(16,00));\
    auto name = vc.createVariable("name", {}, provider, range);\

Q_DECLARE_METATYPE(DateTimeRangeTransformation);


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
        TEST_VC2_FIXTURE(2);
        auto range = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(14,00),
                                                 QDate(2018,8,7),QTime(16,00));
        bool callbackCalled = false;
        connect(&vc,&VariableController2::variableAdded, [&callbackCalled](std::shared_ptr<Variable>){callbackCalled=true;});
        QVERIFY(!callbackCalled);
        auto var1 = vc.createVariable("var1", {}, provider, range);
        QVERIFY(SciQLop::containers::contains(vc.variables(), var1));
        QVERIFY(callbackCalled);
    }

    void testDeleteVariable()
    {
        TEST_VC2_FIXTURE(1);
        auto range = DateTimeRange::fromDateTime(QDate(2018,8,7),QTime(14,00),
                                                 QDate(2018,8,7),QTime(16,00));
        bool callbackCalled = false;
        connect(&vc,&VariableController2::variableDeleted, [&callbackCalled](std::shared_ptr<Variable>){callbackCalled=true;});
        auto var1 = vc.createVariable("var1", {}, provider, range);
        QVERIFY(SciQLop::containers::contains(vc.variables(), var1));
        QVERIFY(!callbackCalled);
        vc.deleteVariable(var1);
        QVERIFY(!SciQLop::containers::contains(vc.variables(), var1));
        QVERIFY(callbackCalled);
    }

    void testGetData()
    {
        TEST_VC2_FIXTURE(10);
        TEST_VC2_CREATE_DEFAULT_VAR(var1);
        check_variable_state<RangeType<10>>(var1, range);
    }

    void testZoom_data()
    {
        QTest::addColumn<double>("zoom");
        QTest::newRow("Zoom IN 10x") << .1;
        QTest::newRow("Zoom OUT 10x")  << 10.;
        QTest::newRow("Zoom IN 1x")  << 1.;
    }
    void testZoom()
    {
        TEST_VC2_FIXTURE(100);
        TEST_VC2_CREATE_DEFAULT_VAR(var1);
        check_variable_state<RangeType<100>>(var1, range);

        QFETCH(double, zoom);
        range *=zoom;
        vc.changeRange(var1, range);
        check_variable_state<RangeType<100>>(var1, range);
    }

    void testPan_data()
    {
        QTest::addColumn<double>("pan");
        QTest::newRow("Right 1000 seconds") << 1000.;
        QTest::newRow("Left 1000 seconds")  << -1000.;
        QTest::newRow("Right 0.1 seconds") << .1;
        QTest::newRow("Left 0.1 seconds")  << -.1;
    }
    void testPan()
    {
        TEST_VC2_FIXTURE(10);
        TEST_VC2_CREATE_DEFAULT_VAR(var1);
        check_variable_state<RangeType<10>>(var1, range);

        QFETCH(double, pan);

        range += Seconds<double>{pan};
        vc.changeRange(var1, range);
        check_variable_state<RangeType<10>>(var1, range);
    }

    void testCache_data()
    {
        QTest::addColumn<DateTimeRangeTransformation>("transformation");
        QTest::addColumn<int>("expectedIncrement");
        QTest::newRow("zoom in") << DateTimeRangeTransformation{0.8,Seconds<double>(0.)} << 0;
        QTest::newRow("tiny zoom out")  << DateTimeRangeTransformation{1.01,Seconds<double>(0.)} << 0;
        QTest::newRow("tiny pan left") << DateTimeRangeTransformation{1.,Seconds<double>(-100.)} << 0;
        QTest::newRow("tiny pan right")  << DateTimeRangeTransformation{1.,Seconds<double>(100.)} << 0;
    }
    void testCache()
    {
        TEST_VC2_FIXTURE(10);
        TEST_VC2_CREATE_DEFAULT_VAR(var1);
        check_variable_state<RangeType<10>>(var1, range);

        QFETCH(DateTimeRangeTransformation, transformation);
        QFETCH(int, expectedIncrement);
        auto initialCount = provider->callCounter;
        range = range.transform(transformation);
        vc.changeRange(var1, range);
        check_variable_state<RangeType<10>>(var1, range);
        QCOMPARE(provider->callCounter-initialCount, expectedIncrement);
    }
};


QTEST_MAIN(TestVariableController2)

#include "TestVariableController2.moc"

