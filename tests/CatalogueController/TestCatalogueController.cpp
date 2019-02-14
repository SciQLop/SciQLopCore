#include <Catalogue/CatalogueController.h>
#include <QObject>
#include <QtTest>
#include <TestUtils/TestProviders.h>


class TestCatalogueController : public QObject
{
    Q_OBJECT
public:
    explicit TestCatalogueController(QObject* parent = nullptr) : QObject(parent) {}
signals:

private slots:
    void initTestCase() {}
    void cleanupTestCase() {}
};


QTEST_MAIN(TestCatalogueController)

#include "TestCatalogueController.moc"
