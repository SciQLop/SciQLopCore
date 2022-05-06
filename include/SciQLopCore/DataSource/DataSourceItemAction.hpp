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
#pragma once

#include <QLoggingCategory>
#include <QObject>
#include <QString>

#include <functional>
#include <memory>

Q_DECLARE_LOGGING_CATEGORY(LOG_DataSourceItemAction)

class DataSourceItem;

/**
 * @brief The DataSourceItemAction class represents an action on a data source item.
 *
 * An action is a function that will be executed when the slot execute() is called.
 */
class DataSourceItemAction : public QObject {

    Q_OBJECT

public:
    /// Signature of the function associated to the action
    using ExecuteFunction = std::function<void(DataSourceItem &dataSourceItem)>;

    /**
     * Ctor
     * @param name the name of the action
     * @param fun the function that will be called when the action is executed
     * @sa execute()
     */
    explicit DataSourceItemAction(const QString &name, ExecuteFunction fun);
    ~DataSourceItemAction();

    std::unique_ptr<DataSourceItemAction> clone() const;

    QString name() const noexcept;

    /// Sets the data source item concerned by the action
    void setDataSourceItem(DataSourceItem *dataSourceItem) noexcept;

public slots:
    /// Executes the action
    void execute();

private:
    class DataSourceItemActionPrivate;
    std::unique_ptr<DataSourceItemActionPrivate> impl;
};
