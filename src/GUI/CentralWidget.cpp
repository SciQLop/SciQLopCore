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

void CentralWidget::addPanel(TimeSyncPanel *panel)
{
    if(panel)
    {
      auto doc = new QDockWidget(this);
      doc->setAttribute(Qt::WA_DeleteOnClose);
      doc->setAllowedAreas(Qt::AllDockWidgetAreas);
      doc->setWidget(panel);
      panel->setParent(doc);
      this->addDockWidget(Qt::DockWidgetArea::TopDockWidgetArea, doc);
      doc->setWindowTitle(panel->name());
      this->_panels.append(panel);
      connect(panel,&TimeSyncPanel::destroyed,this,[this,panel](){this->removePanel(panel);});
      qCDebug(gui_logs) << "TimeSyncPanel added";
      emit this->panels_list_changed(panels());
    }
}

void CentralWidget::removePanel(TimeSyncPanel *panel)
{
    this->_panels.removeAll(panel);
    emit this->panels_list_changed(panels());
}

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

void CentralWidget::addTimeSyncPanel(TimeSyncPanel* panel)
{
  addPanel(panel);
}

void CentralWidget::plot(const QStringList& products)
{
  auto p = new TimeSyncPanel{};
  addTimeSyncPanel(p);
  p->plot(products);
}

TimeSyncPanel *CentralWidget::plotPanel(const QString &name)
{
    for(auto p:this->_panels)
    {
        if(p->name()==name)
            return p;
    }
    return nullptr;
}

QStringList CentralWidget::panels() const
{
    QStringList ps;
    for(const auto p:_panels)
    {
        ps << p->name();
    }
    return ps;
}

DropHelper_default_def(CentralWidget, d_helper)
