#include <QObject>
#include <QtTest>
#include <Variable/Variable2.h>
#include <memory>

class TestVariable : public QObject
{
  Q_OBJECT

private slots:
  void initTestCase() {}
  void type_matches_TS_type()
  {
    Variable2 v("none", {});
    QVERIFY(v.type() == DataSeriesType::NONE);
  }
};

QTEST_MAIN(TestVariable)
#include "TestVariable.moc"
