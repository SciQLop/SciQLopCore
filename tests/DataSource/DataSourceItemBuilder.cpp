#include "DataSourceItemBuilder.h"

DataSourceItemBuilder &DataSourceItemBuilder::root(const QString &name)
{
    m_Root = std::make_shared<DataSourceItem>(DataSourceItemType::NODE, name);
    m_Items.push(m_Root.get());
    return *this;
}

DataSourceItemBuilder &DataSourceItemBuilder::root(QVariantHash data)
{
    m_Root = std::make_shared<DataSourceItem>(DataSourceItemType::NODE,data[DataSourceItem::NAME_DATA_KEY].toString(), data);
    m_Items.push(m_Root.get());
    return *this;
}

DataSourceItemBuilder &DataSourceItemBuilder::node(const QString &name)
{
    return append(DataSourceItemType::NODE, name);
}

DataSourceItemBuilder &DataSourceItemBuilder::node(QVariantHash data)
{
    return append(DataSourceItemType::NODE,data[DataSourceItem::NAME_DATA_KEY].toString(), std::move(data));
}

DataSourceItemBuilder &DataSourceItemBuilder::product(const QString &name)
{
    return append(DataSourceItemType::PRODUCT, name);
}

DataSourceItemBuilder &DataSourceItemBuilder::product(QVariantHash data)
{
    return append(DataSourceItemType::PRODUCT,data[DataSourceItem::NAME_DATA_KEY].toString(), std::move(data));
}

DataSourceItemBuilder &DataSourceItemBuilder::component(const QString &name)
{
    return append(DataSourceItemType::COMPONENT, name);
}

DataSourceItemBuilder &DataSourceItemBuilder::component(QVariantHash data)
{
    return append(DataSourceItemType::COMPONENT,data[DataSourceItem::NAME_DATA_KEY].toString(), std::move(data));
}

DataSourceItemBuilder &DataSourceItemBuilder::end()
{
    m_Items.pop();
    return *this;
}

std::shared_ptr<DataSourceItem> DataSourceItemBuilder::build()
{
    return m_Root;
}

DataSourceItemBuilder &DataSourceItemBuilder::append(DataSourceItemType type, const QString &name)
{
    append(type,name, QVariantHash{{DataSourceItem::NAME_DATA_KEY, name}});
    return *this;
}

DataSourceItemBuilder &DataSourceItemBuilder::append(DataSourceItemType type, const QString &name, QVariantHash data)
{
    auto parentItem = m_Items.top();

    auto insertIndex = parentItem->childCount();
    parentItem->appendChild(std::make_unique<DataSourceItem>(type,name, std::move(data)));

    m_Items.push(parentItem->child(insertIndex));
    return *this;
}
