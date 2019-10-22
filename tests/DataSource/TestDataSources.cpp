#include <DataSource/datasources.h>
#include <DataSource/DataSourceItem.h>

#include <QObject>
#include <QtTest>

class TestDataSources : public QObject {
    Q_OBJECT
private slots:
    void addItems();
};

void TestDataSources::addItems()
{
    DataSources ds;
}

QTEST_MAIN(TestDataSources)
#include "TestDataSources.moc"
