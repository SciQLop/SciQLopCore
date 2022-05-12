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
#include <QTabWidget>
#include <SciQLopCore/GUI/CentralWidget.hpp>
#include <SciQLopCore/GUI/TimeSyncPannel.hpp>
#include <iostream>

CentralWidget::CentralWidget(QWidget* parent) : QMainWindow{parent}
{
  setWindowFlags(Qt::Widget);
  setWindowTitle("Plot area");
  setDockNestingEnabled(true);
}

void CentralWidget::addTimeSynPannel(TimeSyncPannel* pannel)
{
  if(pannel)
  {
    auto doc = new QDockWidget(this);
    doc->setAllowedAreas(Qt::AllDockWidgetAreas);
    doc->setWidget(pannel);
    this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, doc);
    std::cout << "TimeSyncPannel added" << std::endl;
  }
}
