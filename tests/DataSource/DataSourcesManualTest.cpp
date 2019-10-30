#include "DataSource/datasources.h"
#include "TestProviders.h"

#include <QApplication>
#include <QFileIconProvider>
#include <QLineEdit>
#include <QObject>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>
#include <cpp_utils.hpp>
#include <iostream>

void fillModel(DataSources* ds, SimpleRange<1>* provider)
{
    ds->addIcon("folder", QFileIconProvider().icon(QFileIconProvider::Folder));
    ds->addIcon("computer",
                QFileIconProvider().icon(QFileIconProvider::Computer));
    ds->addProvider(provider);
    ds->addDataSourceItem(*provider, "/test/node1", {{"Key", "Value"}});
    ds->addDataSourceItem(*provider, "/test/node2", {});
    ds->addDataSourceItem(*provider, "/test/node3", {});
    ds->addDataSourceItem(*provider, "/test/node4", {});
    ds->setIcon("/test", "folder");
    ds->addDataSourceItem(*provider, "/test/folder/node1", {});
    ds->addDataSourceItem(*provider, "/test2/folder/node1", {});
    repeat_n(
        [ds, provider](int i) {
            repeat_n(
                [ds, provider, i](int j) {
                    repeat_n(
                        [ds, provider, i, j](int k) {
                            ds->addDataSourceItem(*provider,
                                                  "/test2/folder" + QString::number(i) +
                                                  "/folder" + QString::number(j) +
                                                  "/node" + QString::number(k),
                                                  {});
                        },
                        10);
                },
                10);
        },
        10);
    
    ds->updateNodeMetaData("/test2/folder", {{"KeyUp", "ValueUp"}});
    ds->setIcon("/test2", "computer");
}

int main(int argc, char** argv)
{
  SimpleRange<1>* provider = new SimpleRange<1>{};
  QApplication app(argc, argv);
  auto w          = new QWidget();
  auto layout     = new QVBoxLayout(w);
  DataSources* ds = new DataSources();
  // DataSourceFilter* filter = new DataSourceFilter();
  QSortFilterProxyModel* filter = new QSortFilterProxyModel();
  filter->setSourceModel(ds);
  QTreeView* view      = new QTreeView();
  QLineEdit* filterBox = new QLineEdit();
  QObject::connect(
      filterBox,
      static_cast<void (QLineEdit::*)(const QString&)>(&QLineEdit::textChanged),
      filter,
      static_cast<void (QSortFilterProxyModel::*)(const QString&)>(
          &QSortFilterProxyModel::setFilterRegExp));
  layout->addWidget(filterBox);
  layout->addWidget(view);
  view->setModel(filter);
  w->show();
  view->setDragEnabled(true);
  filter->setFilterRole(Qt::ToolTipRole);
  filter->setRecursiveFilteringEnabled(true);
  fillModel(ds, provider);
  view->expandAll();
  return app.exec();
}
