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
#include "SciQLopCore/DataSource/DataSourceItemMergeHelper.hpp"

#include "SciQLopCore/DataSource/DataSourceItem.hpp"

namespace
{
  /**
   * Finds in a tree an item similar to the item passed in parameter
   * @param item the item for which to find a similar item
   * @param root the root item of the tree
   * @return the similar item if found, nullptr otherwise
   */
  DataSourceItem* findSimilarItem(const DataSourceItem& item,
                                  const DataSourceItem& root)
  {
    // An item is considered similar to the another item if:
    // - the items are both nodes AND
    // - the names of the items are identical

    if(item.type() != DataSourceItemType::NODE) { return nullptr; }

    auto similar_item = std::find_if(
        std::cbegin(root), std::cend(root), [&item](const auto& child) {
          return child->type() == DataSourceItemType::NODE &&
                 QString::compare(child->name(), item.name(),
                                  Qt::CaseInsensitive) == 0;
        });
    if(similar_item == std::cend(root)) return nullptr;
    return similar_item->get();
  }

} // namespace

void DataSourceItemMergeHelper::merge(const DataSourceItem& source,
                                      DataSourceItem& dest)
{
  // Checks if the source item can be merged into the destination item (i.e.
  // there is a child item similar to the source item)
  if(auto subItem = findSimilarItem(source, dest))
  {
    // If there is an item similar to the source item, applies the merge
    // recursively
    std::for_each(std::cbegin(source), std::cend(source),
                  [subItem](auto& child) { merge(*child.get(), *subItem); });
  }
  else
  {
    // If no item is similar to the source item, the item is copied as the child
    // of the destination item
    if(std::cend(dest) ==
       std::find_if(std::cbegin(dest), std::cend(dest),
                    [&source](const auto& child) {
                      return source.type() != DataSourceItemType::NODE &&
                             source.name() == child->name() &&
                             source.data() == child->data();
                    }))
      dest.appendChild(source.clone());
  }
}
