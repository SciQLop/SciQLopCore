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

#include <QWidget>
#include <QWidgetAction>

#include "SciQLopCore/Data/DateTimeRange.hpp"


namespace Ui
{
class TimeWidget;
} // Ui


class TimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TimeWidget(QWidget* parent = 0);
    virtual ~TimeWidget();

    void setTimeRange(DateTimeRange time);
    DateTimeRange timeRange() const;

signals:
    /// Signal emitted when the time parameters has beed updated
    void timeUpdated(DateTimeRange time);


protected:
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::TimeWidget* ui;
};

class TimeWidgetAction : public QWidgetAction
{
    Q_OBJECT
    TimeWidget* timeWidget;

public:
    explicit TimeWidgetAction(QWidget* parent = 0) : QWidgetAction(parent)
    {
        timeWidget = new TimeWidget();
        this->setDefaultWidget(timeWidget);
    }
};

