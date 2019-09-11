#include <containers/algorithms.hpp>
#include <Data/DataProviderParameters.h>
#include <Data/DateTimeRange.h>
#include <Data/IDataProvider.h>
#include <Data/ScalarTimeSerie.h>
#include <QObject>
#include <QtTest>
#include <TestUtils/TestProviders.h>
#include <Variable/VariableController2.h>
#include <algorithm>
#include <cmath>
#include <numeric>

#define TEST_VC2_FIXTURE(slope)                                                \
  VariableController2 vc;                                                      \
  auto provider = std::make_shared<SimpleRange<slope>>();

#define TEST_VC2_CREATE_DEFAULT_VARS(name1, name2, name3)                      \
  auto range = DateTimeRange::fromDateTime(QDate(2018, 8, 7), QTime(14, 00),   \
                                           QDate(2018, 8, 7), QTime(16, 00));  \
  auto name1 = vc.createVariable("name1", {}, provider, range);                \
  auto name2 = vc.createVariable("name1", {}, provider, range);                \
  auto name3 = vc.createVariable("name1", {}, provider, range);                \
  vc.synchronize(name1, name2);

class TestVariableController2Async : public QObject
{
  Q_OBJECT
public:
  explicit TestVariableController2Async(QObject* parent = nullptr)
      : QObject(parent)
  {}
signals:

private slots:
  void initTestCase() {}
  void cleanupTestCase() {}

  void testSimplePan()
  {
    TEST_VC2_FIXTURE(2);
    auto range = DateTimeRange::fromDateTime(QDate(2018, 8, 7), QTime(14, 00),
                                             QDate(2018, 8, 7), QTime(16, 00));
    int variableUpdated = 0;
    auto var1           = vc.createVariable("var1", {}, provider, range);
    auto var2           = vc.createVariable("var2", {}, provider, range);
    auto var3           = vc.createVariable("var3", {}, provider, range);
    connect(&(*var2), &Variable2::updated,
            [&variableUpdated]() { variableUpdated += 1; });
    vc.synchronize(var1, var2);
    vc.asyncChangeRange(var1, range + Seconds<double>{10000.});
    vc.asyncChangeRange(var1, range + Seconds<double>{50000.});
    vc.asyncChangeRange(var1, range + Seconds<double>{100000.});
    vc.asyncChangeRange(var1, range + Seconds<double>{150000.});
    while(!vc.isReady(var1) || !vc.isReady(var2))
    {
      QCoreApplication::processEvents();
    }
  }
};

QTEST_MAIN(TestVariableController2Async)

#include "TestVariableController2Async.moc"
