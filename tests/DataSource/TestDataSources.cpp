#include <DataSource/datasources.h>
#include <DataSource/DataSourceItem.h>

#include <QObject>
#include <QtTest>
#include "TestProviders.h"

class TestDataSources : public QObject {
    Q_OBJECT
private slots:
    void addItems();
};

void TestDataSources::addItems()
{
    SimpleRange<1>* provider = new SimpleRange<1>{};
    DataSources ds;
    ds.addProvider(provider);
    ds.addDataSourceItem(provider->id(),"/test/node1",{});
    ds.addDataSourceItem(provider->id(),"/test/node2",{});
    ds.addDataSourceItem(provider->id(),"/test/node3",{});
    ds.addDataSourceItem(provider->id(),"/test/node4",{});
    auto rc = ds.rowCount(ds.index(0,0,QModelIndex()));
    QVERIFY(rc==4);
}

QTEST_MAIN(TestDataSources)
#include "TestDataSources.moc"
