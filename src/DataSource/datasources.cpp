/*------------------------------------------------------------------------------
 * --  This file is a part of SciQLop
 * --  Copyright (C) 2019, Plasma Physics Laboratory - CNRS
 * --
 * --  This program is free software; you can redistribute it and/or modify
 * --  it under the terms of the GNU General Public License as published by
 * --  the Free Software Foundation; either version 3 of the License, or
 * --  (at your option) any later version.
 * --
 * --  This program is distributed in the hope that it will be useful,
 * --  but WITHOUT ANY WARRANTY; without even the implied warranty of
 * --  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * --  GNU General Public License for more details.
 * --
 * --  You should have received a copy of the GNU General Public License
 * --  along with this program; if not, write to the Free Software
 * --  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -------------------------------------------------------------------------------*/
/*--                  Author : Alexis Jeandet
 * --                     Mail : alexis.jeandet@lpp.polytechnique.fr
 * --                            alexis.jeandet@member.fsf.org
 * ----------------------------------------------------------------------------*/
// https://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html

#include "DataSource/datasources.h"

inline std::unique_ptr<DataSourceItem> make_folder_item(const QString& name)
{
  return std::make_unique<DataSourceItem>(DataSourceItemType::NODE, name);
}

template<typename T>
DataSourceItem* make_path_items(const T& path_list_begin,
                                const T& path_list_end, DataSourceItem* root)
{
  std::for_each(path_list_begin, path_list_end,
                [&root](const auto& folder_name) mutable {
                  auto folder_ptr = root->findItem(folder_name);
                  if(folder_ptr == nullptr)
                  {
                    auto folder = make_folder_item(folder_name);
                    folder_ptr  = folder.get();
                    root->appendChild(std::move(folder));
                  }
                  root = folder_ptr;
                });
  return root;
}

inline std::unique_ptr<DataSourceItem>
make_product_item(const QVariantHash& metaData, const QUuid& dataSourceUid,
                  const QString& DATA_SOURCE_NAME, DataSources* dc)
{
  auto result =
      std::make_unique<DataSourceItem>(DataSourceItemType::PRODUCT, metaData);

  // Adds plugin name to product metadata
  result->setData(DataSourceItem::PLUGIN_DATA_KEY, DATA_SOURCE_NAME);
  result->setData(DataSourceItem::ID_DATA_KEY,
                  metaData.value(DataSourceItem::NAME_DATA_KEY));

  auto productName = metaData.value(DataSourceItem::NAME_DATA_KEY).toString();

  // Add action to load product from DataSourceController
  //   result->addAction(std::make_unique<DataSourceItemAction>(
  //       QObject::tr("Load %1 product").arg(productName),
  //       [productName, dataSourceUid, dc](DataSourceItem& item) {
  //         if(dc) { dc->loadProductItem(dataSourceUid, item); }
  //       }));

  return result;
}

QVariant DataSources::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) return QVariant();
  if(role != Qt::DisplayRole) return QVariant();
  DataSourceItem* item = static_cast<DataSourceItem*>(index.internalPointer());
  return item->name();
}

int DataSources::columnCount(const QModelIndex& parent) const
{
  (void)parent;
  return 0;
}

int DataSources::rowCount(const QModelIndex& parent) const
{
  DataSourceItem* parentItem;
  if(parent.column() > 0) return 0;
  if(!parent.isValid())
    parentItem = _root;
  else
    parentItem = static_cast<DataSourceItem*>(parent.internalPointer());
  return parentItem->childCount();
}

QModelIndex DataSources::parent(const QModelIndex& index) const
{
  if(!index.isValid()) return QModelIndex();
  DataSourceItem* childItem =
      static_cast<DataSourceItem*>(index.internalPointer());
  DataSourceItem* parentItem = childItem->parentItem();
  if(parentItem == _root) return QModelIndex();
  return createIndex(parentItem->index(), 0, parentItem);
}

QModelIndex DataSources::index(int row, int column,
                               const QModelIndex& parent) const
{
  if(!hasIndex(row, column, parent)) return QModelIndex();
  DataSourceItem* parentItem;
  if(!parent.isValid())
    parentItem = _root;
  else
    parentItem = static_cast<DataSourceItem*>(parent.internalPointer());
  DataSourceItem* childItem = parentItem->child(row);
  if(childItem) return createIndex(row, column, childItem);
  return QModelIndex();
}

void DataSources::addDataSourceItem(
    const QUuid& providerUid, const QString& path,
    const QMap<QString, QString>& metaData) noexcept
{
  auto path_list = path.split('/', QString::SkipEmptyParts);
  auto name      = *(std::cend(path_list) - 1);
  auto path_item =
      make_path_items(std::cbegin(path_list), std::cend(path_list) - 1, _root);
  QVariantHash meta_data{{DataSourceItem::NAME_DATA_KEY, name}};
  for(auto& key : metaData.keys())
  {
    meta_data[key] = metaData[key];
  }
  path_item->appendChild(
      make_product_item(meta_data, providerUid, "test", this));
}

void DataSources::addProvider(IDataProvider* provider) noexcept
{
  _DataProviders.insert(
      {provider->id(), std::unique_ptr<IDataProvider>{provider}});
}
