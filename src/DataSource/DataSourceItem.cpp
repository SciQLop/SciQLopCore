/*------------------------------------------------------------------------------
-- This file is a part of the SciQLop Software
-- Copyright (C) 2022, Plasma Physics Laboratory - CNRS
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
-------------------------------------------------------------------------------*/
/*-- Author : Alexis Jeandet
-- Mail : alexis.jeandet@member.fsf.org
----------------------------------------------------------------------------*/
#include "SciQLopCore/DataSource/DataSourceItem.hpp"

#include "SciQLopCore/DataSource/DataSourceItemAction.hpp"
#include "SciQLopCore/DataSource/DataSourceItemMergeHelper.hpp"

#include <QUuid>
#include <QVector>
#include <containers/algorithms.hpp>
#include <optional>

const QString DataSourceItem::NAME_DATA_KEY   = QStringLiteral("name");
const QString DataSourceItem::PLUGIN_DATA_KEY = QStringLiteral("plugin");
const QString DataSourceItem::ID_DATA_KEY     = QStringLiteral("uuid");

struct DataSourceItem::DataSourceItemPrivate
{
  explicit DataSourceItemPrivate(DataSourceItemType type, const QString& name,
                                 DataSeriesType ds_type, QVariantHash data,
                                 std::optional<QUuid> sourceUUID = std::nullopt)
      : m_Parent{nullptr}, m_dataSourceUid{sourceUUID},
        m_Children{}, m_name{name}, m_ds_type{ds_type}, m_Type{type},
        m_Data{std::move(data)}, m_Actions{}
  {
    m_Data[DataSourceItem::NAME_DATA_KEY] = name;
  }
  ~DataSourceItemPrivate();
  DataSourceItem* m_Parent;
  std::optional<QUuid> m_dataSourceUid = std::nullopt;
  std::vector<std::unique_ptr<DataSourceItem>> m_Children;
  QString m_icon;
  QString m_name;
  DataSeriesType m_ds_type;
  DataSourceItemType m_Type;

  // TODO check if QVariant is really wise here, looks quite overkill
  // maybe a simple map<string,string> could be enough
  QVariantHash m_Data;
  std::vector<std::unique_ptr<DataSourceItemAction>> m_Actions;
  auto begin() noexcept { return m_Children.begin(); }
  auto end() noexcept { return m_Children.end(); }
  auto cbegin() const noexcept { return m_Children.cbegin(); }
  auto cend() const noexcept { return m_Children.cend(); }
  inline std::optional<QUuid> source_uuid() const noexcept
  {
    return m_dataSourceUid;
  }
  int index_of(const DataSourceItem* item)
  {
    return std::distance(std::cbegin(m_Children),
                         std::find_if(std::cbegin(m_Children),
                                      std::cend(m_Children),
                                      [item](const auto& other) {
                                        return other.get() == item;
                                      }));
  }
  inline QString name() const noexcept { return m_name; }
  inline QString icon() const noexcept { return m_icon; }
  inline void setIcon(const QString& iconName) { m_icon = iconName; }
  inline DataSeriesType dataSeriesType() { return m_ds_type; }
};

// DataSourceItem::DataSourceItem(DataSourceItemType type, const QString& name)
//    : DataSourceItem{type,name, QVariantHash{{NAME_DATA_KEY, name}}}
//{}

DataSourceItem::DataSourceItem(DataSourceItemType type, const QString& name,
                               DataSeriesType ds_type, QVariantHash data,
                               std::optional<QUuid> sourceUUID)
    : impl{std::make_unique<DataSourceItemPrivate>(type, name, ds_type,
                                                   std::move(data), sourceUUID)}
{}

DataSourceItem::~DataSourceItem() = default;

// TODO remove this method ASAP
std::unique_ptr<DataSourceItem> DataSourceItem::clone() const
{
  auto result = std::make_unique<DataSourceItem>(
      impl->m_Type, impl->m_name, impl->dataSeriesType(), impl->m_Data);

  // Clones children
  for(const auto& child : impl->m_Children)
  {
    result->appendChild(std::move(child->clone()));
  }

  // Clones actions
  for(const auto& action : impl->m_Actions)
  {
    result->addAction(std::move(action->clone()));
  }

  return result;
}

QVector<DataSourceItemAction*> DataSourceItem::actions() const noexcept
{
  auto result = QVector<DataSourceItemAction*>{};

  std::transform(std::cbegin(impl->m_Actions), std::cend(impl->m_Actions),
                 std::back_inserter(result),
                 [](const auto& action) { return action.get(); });

  return result;
}

void DataSourceItem::addAction(
    std::unique_ptr<DataSourceItemAction> action) noexcept
{
  action->setDataSourceItem(this);
  impl->m_Actions.push_back(std::move(action));
}

void DataSourceItem::appendChild(std::unique_ptr<DataSourceItem> child) noexcept
{
  child->impl->m_Parent = this;
  impl->m_Children.push_back(std::move(child));
}

void DataSourceItem::removeChild(DataSourceItem* child) noexcept
{
  if(auto node =
         std::find_if(std::begin(impl->m_Children), std::end(impl->m_Children),
                      [child](const auto& n) { return n.get() == child; });
     node != std::cend(impl->m_Children))
  {
    std::swap(*node, impl->m_Children.back());
    impl->m_Children.resize(std::size(impl->m_Children) - 1);
  }
}

DataSourceItem* DataSourceItem::child(int childIndex) const noexcept
{
  if(childIndex < 0 || childIndex >= childCount()) { return nullptr; }
  else { return impl->m_Children.at(childIndex).get(); }
}

int DataSourceItem::childCount() const noexcept
{
  return impl->m_Children.size();
}

QVariant DataSourceItem::data(const QString& key) const noexcept
{
  return impl->m_Data.value(key);
}

QVariantHash DataSourceItem::data() const noexcept { return impl->m_Data; }

void DataSourceItem::merge(const DataSourceItem& item)
{
  DataSourceItemMergeHelper::merge(item, *this);
}

bool DataSourceItem::isRoot() const noexcept
{
  return impl->m_Parent == nullptr;
}

QString DataSourceItem::name() const noexcept { return impl->name(); }

QString DataSourceItem::icon() const noexcept { return impl->icon(); }

void DataSourceItem::setIcon(const QString& iconName)
{
  impl->setIcon(iconName);
}

QString DataSourceItem::path() const noexcept
{
  std::vector<QString> path_list;
  path_list.push_back(name());
  auto node = parentItem();
  while(node != nullptr)
  {
    path_list.push_back(node->name());
    node = node->parentItem();
  }
  QString path;
  std::for_each(path_list.crbegin(), path_list.crend(),
                [&path](const auto& name) {
                  path.append('/');
                  path.append(name);
                });
  return path;
}

DataSeriesType DataSourceItem::dataSeriesType() const noexcept
{
  return impl->dataSeriesType();
}

DataSourceItem* DataSourceItem::parentItem() const noexcept
{
  return impl->m_Parent;
}

int DataSourceItem::index() const noexcept
{
  if(auto parent = parentItem(); parent)
  {
    return parent->impl->index_of(this);
  }
  return 0;
}

const DataSourceItem& DataSourceItem::rootItem() const noexcept
{
  return isRoot() ? *this : parentItem()->rootItem();
}

void DataSourceItem::setData(const QString& key, const QVariant& value,
                             bool append) noexcept
{
  auto it = impl->m_Data.constFind(key);
  if(append && it != impl->m_Data.constEnd())
  {
    // Case of an existing value to which we want to add to the new value
    if(it->canConvert<QVariantList>())
    {
      auto variantList = it->value<QVariantList>();
      variantList.append(value);

      impl->m_Data.insert(key, variantList);
    }
    else { impl->m_Data.insert(key, QVariantList{*it, value}); }
  }
  else
  {
    // Other cases :
    // - new value in map OR
    // - replacement of an existing value (not appending)
    impl->m_Data.insert(key, value);
  }
}

DataSourceItemType DataSourceItem::type() const noexcept
{
  return impl->m_Type;
}

DataSourceItem* DataSourceItem::findItem(const QVariantHash& data,
                                         bool recursive)
{
  for(const auto& child : impl->m_Children)
  {
    if(child->impl->m_Data == data) { return child.get(); }

    if(recursive)
    {
      if(auto foundItem = child->findItem(data, true)) { return foundItem; }
    }
  }

  return nullptr;
}

DataSourceItem* DataSourceItem::findItem(const QString& name)
{
  auto item =
      std::find_if(std::cbegin(impl->m_Children), std::cend(impl->m_Children),
                   [name](const auto& item) { return item->name() == name; });
  if(item != std::cend(impl->m_Children)) return item->get();
  return nullptr;
}

DataSourceItem* DataSourceItem::findItem(const QString& datasourceIdKey,
                                         bool recursive)
{
  for(const auto& child : impl->m_Children)
  {
    auto childId = child->impl->m_Data.value(ID_DATA_KEY);
    if(childId == datasourceIdKey) { return child.get(); }

    if(recursive)
    {
      if(auto foundItem = child->findItem(datasourceIdKey, true))
      {
        return foundItem;
      }
    }
  }

  return nullptr;
}

bool DataSourceItem::operator==(const DataSourceItem& other)
{
  // Compares items' attributes
  if(std::tie(impl->m_Type, impl->m_Data) ==
     std::tie(other.impl->m_Type, other.impl->m_Data))
  {
    // Compares contents of items' children
    return std::equal(
        std::cbegin(impl->m_Children), std::cend(impl->m_Children),
        std::cbegin(other.impl->m_Children), std::cend(other.impl->m_Children),
        [](const auto& itemChild, const auto& otherChild) {
          return *itemChild == *otherChild;
        });
  }
  else { return false; }
}

bool DataSourceItem::operator!=(const DataSourceItem& other)
{
  return !(*this == other);
}

DataSourceItem::iterator_type DataSourceItem::begin() noexcept
{
  return impl->begin();
}

DataSourceItem::iterator_type DataSourceItem::end() noexcept
{
  return impl->end();
}

DataSourceItem::const_iterator_type DataSourceItem::cbegin() const noexcept
{
  return impl->cbegin();
}

DataSourceItem::const_iterator_type DataSourceItem::cend() const noexcept
{
  return impl->cend();
}

DataSourceItem::const_iterator_type DataSourceItem::begin() const noexcept
{
  return impl->cbegin();
}

DataSourceItem::const_iterator_type DataSourceItem::end() const noexcept
{
  return impl->cend();
}

std::optional<QUuid> DataSourceItem::source_uuid() const noexcept
{
  return impl->source_uuid();
}

DataSourceItem::DataSourceItemPrivate::~DataSourceItemPrivate() = default;
