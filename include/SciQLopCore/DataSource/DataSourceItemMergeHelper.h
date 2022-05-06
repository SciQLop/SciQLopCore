#pragma once
class DataSourceItem;

/**
 * @brief The DataSourceItemMergeHelper struct is used to merge two data source items
 * @sa DataSourceItem::merge()
 */
struct DataSourceItemMergeHelper {
    /// Merges source item into dest item
    static void merge(const DataSourceItem &source, DataSourceItem &dest);
};
