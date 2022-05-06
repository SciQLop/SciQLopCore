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
#include "SciQLopCore/DataSource/DataSourceItemAction.hpp"
#include "SciQLopCore/DataSource/DataSourceItem.hpp"

#include <functional>

Q_LOGGING_CATEGORY(LOG_DataSourceItemAction, "DataSourceItemAction")

struct DataSourceItemAction::DataSourceItemActionPrivate {
    explicit DataSourceItemActionPrivate(const QString &name,
                                         DataSourceItemAction::ExecuteFunction fun)
            : m_Name{name}, m_Fun{std::move(fun)}, m_DataSourceItem{nullptr}
    {
    }

    QString m_Name;
    DataSourceItemAction::ExecuteFunction m_Fun;
    /// Item associated to the action (can be null, in which case the action will not be executed)
    DataSourceItem *m_DataSourceItem;
};

DataSourceItemAction::DataSourceItemAction(const QString &name, ExecuteFunction fun)
        : impl{std::make_unique<DataSourceItemActionPrivate>(name, std::move(fun))}
{
}

DataSourceItemAction::~DataSourceItemAction()=default;

std::unique_ptr<DataSourceItemAction> DataSourceItemAction::clone() const
{
    return std::make_unique<DataSourceItemAction>(impl->m_Name, impl->m_Fun);
}

QString DataSourceItemAction::name() const noexcept
{
    return impl->m_Name;
}

void DataSourceItemAction::setDataSourceItem(DataSourceItem *dataSourceItem) noexcept
{
    impl->m_DataSourceItem = dataSourceItem;
}

void DataSourceItemAction::execute()
{
    if (impl->m_DataSourceItem) {
        impl->m_Fun(*impl->m_DataSourceItem);
    }
    else {
        qCDebug(LOG_DataSourceItemAction())
            << tr("Can't execute action : no item has been associated");
    }
}
