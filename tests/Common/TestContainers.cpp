#include <Common/containers.h>
#include <vector>
#include <set>
#include <list>
#include <QVector>
#include <QList>

#include <QObject>
#include <QtTest>

template<class T>
void test()
{
    T cont{{1,2,3,3,3}};
    T empty;

    QCOMPARE(SciQLop::containers::contains(cont,1),true);
    QCOMPARE(SciQLop::containers::contains(cont,3),true);

    QCOMPARE(SciQLop::containers::contains(cont,-1),false);
    QCOMPARE(SciQLop::containers::contains(empty,1),false);
}

class TestContainers: public QObject {
    Q_OBJECT

private slots:
    void testVector() { test< std::vector<int> >();}
    void testSet()    { test< std::set<int>    >();}
    void testList()   { test< std::list<int>   >();}
    void testQVector(){ test< QVector<int>     >();}
    void testQList()  { test< QList<int>       >();}
};

QTEST_MAIN(TestContainers)
#include "TestContainers.moc"
