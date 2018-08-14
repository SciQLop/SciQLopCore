#include <QObject>
#include <QtTest>
#include <QUuid>

#include <Variable/VariableSynchronizationGroup2.h>
#include <Common/debug.h>

class TestSyncGroup: public QObject {
    Q_OBJECT

private slots:
    void testAddVariables()
    {
        auto v = QUuid::createUuid();
        VariableSynchronizationGroup2 group{v};
        QVERIFY(group.contains(v));
        auto vars = {QUuid::createUuid(), QUuid::createUuid(), QUuid::createUuid(), QUuid::createUuid()};
        for(auto var:vars)
        {
            group.addVariable(var);
        }
        for(auto var:vars)
        {
            QVERIFY(group.contains(var));
        }
    }

    void testAddingTwiceAVar()
    {
        auto v = QUuid::createUuid();
        VariableSynchronizationGroup2 group{v};
        QVERIFY(group.contains(v));
        group.addVariable(v);
        QVERIFY(group.variables().size()==1);
    }

    void testRemoveVariables()
    {
        auto v = QUuid::createUuid();
        VariableSynchronizationGroup2 group{v};
        QVERIFY(group.contains(v));
        group.removeVariable(v);
        QVERIFY(!group.contains(v));
        auto vars = {QUuid::createUuid(), QUuid::createUuid(), QUuid::createUuid(), QUuid::createUuid()};
        for(auto var:vars)
        {
            group.addVariable(var);
        }
        for(auto var:vars)
        {
            QVERIFY(group.contains(var));
            group.removeVariable(var);
            QVERIFY(!group.contains(var));
        }
        //shouldn't crash
        group.removeVariable(v);
    }
};
QTEST_MAIN(TestSyncGroup)

#include "TestSyncGroup.moc"
