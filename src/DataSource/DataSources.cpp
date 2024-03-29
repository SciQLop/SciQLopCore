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

#include "SciQLopCore/DataSource/DataSources.hpp"

#include "SciQLopCore/DataSource/DataSourceItemAction.hpp"
#include "SciQLopCore/MimeTypes/MimeTypes.hpp"
#include "containers/algorithms.hpp"

#include <QDataStream>
#include <iostream>

QString QVariant2QString(const QVariant& variant) noexcept
{
  if(variant.userType() == QMetaType::QStringList)
  {
    auto valueString = QStringLiteral("{");
    auto variantList = variant.value<QVariantList>();
    QStringList items;
    std::transform(std::cbegin(variantList), std::cend(variantList),
                   std::back_inserter(items),
                   [](const auto& item) { return QVariant2QString(item); });
    valueString.append(cpp_utils::containers::join(items, ", "));
    valueString.append("}");
    return valueString;
  }
  else { return variant.toString(); }
}

inline std::unique_ptr<DataSourceItem> make_folder_item(const QString& name)
{
  return std::make_unique<DataSourceItem>(
      DataSourceItemType::NODE, name, DataSeriesType::NONE, QVariantHash{}, "");
}

template<typename T>
DataSourceItem* walk_tree(
    const T& path_list_begin, const T& path_list_end, DataSourceItem* root,
    const std::function<DataSourceItem*(DataSourceItem*, DataSourceItem*,
                                        const decltype(*std::declval<T>())&)>&
        f = [](DataSourceItem* parent, DataSourceItem* node,
               const auto& name) -> DataSourceItem* {
      (void)parent;
      (void)name;
      return node;
    })
{
  std::for_each(path_list_begin, path_list_end,
                [&root, &f](const auto& folder_name) mutable {
                  auto folder_ptr = root->findItem(folder_name);
                  root            = f(root, folder_ptr, folder_name);
                });
  return root;
}

DataSourceItem* walk_tree(
    const QString& path, DataSourceItem* root,
    const std::function<DataSourceItem*(DataSourceItem*, DataSourceItem*,
                                        const QString&)>& f =
        [](DataSourceItem* parent, DataSourceItem* node,
           const auto& name) -> DataSourceItem* {
      (void)parent;
      (void)name;
      return node;
    })
{
  auto path_list = path.split('/', Qt::SkipEmptyParts);
  return walk_tree(std::cbegin(path_list), std::cend(path_list), root, f);
}

template<typename T>
DataSourceItem* make_path_items(const T& path_list_begin,
                                const T& path_list_end, DataSourceItem* root)
{
  auto node_ctor = [](DataSourceItem* parent, DataSourceItem* node,
                      const auto& name) -> DataSourceItem* {
    if(node == nullptr)
    {
      auto folder = make_folder_item(name);
      node        = folder.get();
      parent->appendChild(std::move(folder));
    }
    return node;
  };
  return walk_tree(path_list_begin, path_list_end, root, node_ctor);
}

inline std::unique_ptr<DataSourceItem>
make_product_item(const QString& name, DataSeriesType ds_type,
                  QVariantHash& metaData, const QString& dataSourceUid,
                  const QString& DATA_SOURCE_NAME, DataSources* dataSources)
{
  auto result = std::make_unique<DataSourceItem>(
      DataSourceItemType::PRODUCT, name, ds_type, metaData, dataSourceUid);

  // Adds plugin name to product metadata
  // TODO re-consider adding a name attribute to DataSourceItem class
  result->setData(DataSourceItem::PLUGIN_DATA_KEY, DATA_SOURCE_NAME);

  // Add action to load product from DataSources
  //  result->addAction(std::make_unique<DataSourceItemAction>(
  //      QObject::tr("Load %1 product").arg(name),
  //      [dataSources](DataSourceItem& item) {
  //        if(dataSources) { dataSources->createVariable(item); }
  //      }));

  return result;
}

DataSources::DataSources()
    : SciQLopObject{this},
      _root(new DataSourceItem(DataSourceItemType::NODE, "",
                               DataSeriesType::NONE, QVariantHash{}, "")),
      _completionModel(new QStringListModel)
{}

DataSources::~DataSources()
{
  if(_root) delete _root;
  if(_completionModel) delete _completionModel;
}

QVariant DataSources::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) return QVariant();
  DataSourceItem* item = static_cast<DataSourceItem*>(index.internalPointer());
  if(role == Qt::DisplayRole) { return item->name(); }
  if(role == Qt::DecorationRole)
  {
    return _icons.value(item->icon(), QVariant{});
  }
  if(role == Qt::ToolTipRole)
  {
    auto result      = QString{};
    const auto& data = item->data();
    std::for_each(data.constKeyValueBegin(), data.constKeyValueEnd(),
                  [&result](const auto& item) {
                    result.append(QString{"<b>%1:</b> %2<br/>"}.arg(
                        item.first, QVariant2QString(item.second)));
                  });
    return result;
  }
  return QVariant();
}

QMimeData* DataSources::mimeData(const QModelIndexList& indexes) const
{
  std::vector<DataSourceItem*> items;
  std::for_each(std::cbegin(indexes), std::cend(indexes),
                [&items](const auto& index) {
                  if(index.isValid())
                  {
                    DataSourceItem* item =
                        static_cast<DataSourceItem*>(index.internalPointer());
                    if(item->isProductOrComponent()) { items.push_back(item); }
                  }
                });
  return MIME::mimeData(items);
}

int DataSources::columnCount(const QModelIndex& parent) const
{
  (void)parent;
  return 1;
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

Qt::ItemFlags DataSources::flags(const QModelIndex& index) const
{
  Qt::ItemFlags flags = Qt::NoItemFlags;
  if(index.isValid())
  {
    flags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    DataSourceItem* item =
        static_cast<DataSourceItem*>(index.internalPointer());
    if(item && item->isProductOrComponent())
    {
      flags |= Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
  }
  return flags;
}

// TODO This can be optimized to only use insert row and column
// this should be much faster than doing a ResetModel all the time
// but this is more difficult to implement
void DataSources::addDataSourceItem(
    const QString& providerUid, const QString& path, DataSeriesType ds_type,
    const QMap<QString, QString>& metaData) noexcept
{
  beginResetModel();
  QSet<QString> completion_data;
  auto path_list = path.split('/', Qt::SkipEmptyParts);
  auto name      = *(std::cend(path_list) - 1);
  auto path_item =
      make_path_items(std::cbegin(path_list), std::cend(path_list) - 1, _root);
  completion_data << name;
  QVariantHash meta_data{{DataSourceItem::NAME_DATA_KEY, name}};
  std::for_each(metaData.keyBegin(), metaData.keyEnd(),
                [&](const auto& key) { meta_data[key] = metaData[key];completion_data << key; });
  path_item->appendChild(
      make_product_item(name, ds_type, meta_data, providerUid, "test", this));
  endResetModel();
  _updateCompletionModel(completion_data);
  _Products[providerUid].append(path);
}

void DataSources::addProducts(const QString& providerUid,
                              const QVector<Product*>& products)
{
  beginResetModel();
  QSet<QString> completion_data;
  for(const auto product : products)
  {
    auto path_list = product->path.split('/', Qt::SkipEmptyParts);
    auto name      = *(std::cend(path_list) - 1);
    auto path_item = make_path_items(std::cbegin(path_list),
                                     std::cend(path_list) - 1, _root);
    QVariantHash meta_data{{DataSourceItem::NAME_DATA_KEY, name}};
    completion_data << name;
    std::for_each(
        product->metadata.keyBegin(), product->metadata.keyEnd(),
        [&](const auto& key) {
        meta_data[key] = product->metadata[key];
        completion_data << key;
    });
    path_item->appendChild(make_product_item(name, product->ds_type, meta_data,
                                             providerUid, "test", this));
    _Products[providerUid].append(product->path);
  }
  _updateCompletionModel(completion_data);
  endResetModel();
}

void DataSources::removeDataSourceItems(const QStringList& paths) noexcept
{
  beginResetModel();
  for(const auto& path : paths)
  {
    auto node = walk_tree(path, _root);
    if(node != nullptr) { node->parentItem()->removeChild(node); }
  }
  endResetModel();
}

void DataSources::addProvider(IDataProvider* provider) noexcept
{
  _DataProviders.insert({provider->name(), provider});
  _Products[provider->name()] = QStringList{};
}

void DataSources::removeProvider(IDataProvider* provider) noexcept
{
  assert(cpp_utils::containers::contains(_Products, provider->name()));
  removeDataSourceItems(_Products[provider->name()]);
  _DataProviders.erase(provider->name());
  _Products.erase(provider->name());
}

void DataSources::setIcon(const QString& path, const QString& iconName)
{
  auto node = walk_tree(path, _root);
  if(node != nullptr) { node->setIcon(iconName); }
}

IDataProvider* DataSources::provider(const QString& path)
{
  auto node = walk_tree(path, _root);
  if(node != nullptr)
  {
    if(auto ds_uuid = node->source_uuid();
       !ds_uuid.isEmpty() && node->isProductOrComponent())
    {
      if(auto data_source_it = _DataProviders.find(ds_uuid);
         data_source_it != std::cend(_DataProviders))
        return data_source_it->second;
    }
  }
  return nullptr;
}

DataSeriesType DataSources::dataSeriesType(const QString& path)
{
  auto node = walk_tree(path, _root);
  if(node != nullptr) { return node->dataSeriesType(); }
  return DataSeriesType::NONE;
}

QVariantHash DataSources::nodeData(const QString& path)
{
  auto node = walk_tree(path, _root);
  if(node != nullptr) { return node->data(); }
  return {};
}

void DataSources::_updateCompletionModel(const QSet<QString> new_data)
{
  auto currentList = _completionModel->stringList();
  std::for_each(new_data.cbegin(), new_data.cend(),
                [&currentList](const auto& value) {
                  currentList << value;
                });
  currentList.removeDuplicates();
  _completionModel->setStringList(currentList);
}
