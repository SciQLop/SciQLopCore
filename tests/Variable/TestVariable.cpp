#include <Variable/Variable.h>

#include <Data/ScalarSeries.h>

#include <QObject>
#include <QtTest>

#include <memory>



class TestVariable : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() { QSKIP("Please Write me! .·´¯`(>▂<)´¯`·. "); }
};




QTEST_MAIN(TestVariable)
#include "TestVariable.moc"
