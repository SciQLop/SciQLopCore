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
#include <SciQLopCore/GUI/TimeSyncPanel.hpp>
#include <SciQLopCore/logging/SciQLopLogs.hpp>
#include <iostream>

CentralWidget::CentralWidget(QWidget* parent)
    : QMainWindow{parent}, SciQLopObject{this},
      d_helper{
          {{MIME::IDS::PRODUCT_LIST,
            [this,
             mime = MIME::txt(MIME::IDS::PRODUCT_LIST)](const QMimeData* data) {
              this->plot(MIME::mimeDataTo(data, mime));
              return true;
            }}}}
{
  setWindowFlags(Qt::Widget);
  setWindowTitle("Plot area");
  setDockNestingEnabled(true);
  this->setAcceptDrops(true);
}

void CentralWidget::addTimeSyncPannel(TimeSyncPanel* pannel)
{
  if(pannel)
  {
    auto doc = new QDockWidget(this);
    doc->setAttribute(Qt::WA_DeleteOnClose);
    doc->setAllowedAreas(Qt::AllDockWidgetAreas);
    doc->setWidget(pannel);
    this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, doc);
    doc->setWindowTitle(pannel->name());
    this->dockWidgets.append(doc);
    qCDebug(gui_logs) << "TimeSyncPanel added";
  }
}

void CentralWidget::plot(const QStringList& products)
{
  auto p = new TimeSyncPanel{};
  addTimeSyncPannel(p);
  p->plot(products);
}

DropHelper_default_def(CentralWidget, d_helper)
