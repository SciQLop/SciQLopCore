#include "SciQLopCore/DataSource/DataSourceItemMergeHelper.h"
#include "SciQLopCore/DataSource/DataSourceItem.h"

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
    if(std::cend(dest) == std::find_if(std::cbegin(dest), std::cend(dest),
                                       [&source](const auto& child)
                                       {return source.type() != DataSourceItemType::NODE && source.name()==child->name() && source.data()==child->data();}))
    dest.appendChild(source.clone());
  }
}
