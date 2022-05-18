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
#include "SciQLopCore/GUI/TimeSyncPanel.hpp"

#include "SciQLopCore/Data/Pipelines.hpp"
#include "SciQLopCore/GUI/PlotWidget.hpp"
#include "SciQLopCore/MimeTypes/MimeTypes.hpp"
#include "SciQLopCore/SciQLopCore.hpp"
#include "SciQLopCore/logging/SciQLopLogs.hpp"
#include "SciQLopPlots/Qt/Graph.hpp"

#include <QDragEnterEvent>
#include <QMimeData>
#include <iostream>

TimeSyncPanel::TimeSyncPanel(QWidget* parent)
    : SciQLopPlots::SyncPannel{parent}, SciQLopObject{this},
      d_helper{
          {{MIME::IDS::TIME_RANGE,
            [this](const QMimeData*) {
              this->setXRange({{}, {}});
              return true;
            }},
           {MIME::IDS::PRODUCT_LIST,
            [this,
             mime = MIME::txt(MIME::IDS::PRODUCT_LIST)](const QMimeData* data) {
              this->plot(MIME::ToQStringList(MIME::decode(data->data(mime))));
              return true;
            }}}}

{
  setAcceptDrops(true);
  setXRange({(QDateTime::currentSecsSinceEpoch() - 3600 * 24 * 700) * 1.,
             (QDateTime::currentSecsSinceEpoch() - 3600 * 24 * 699) * 1.});
}

TimeSyncPanel::~TimeSyncPanel()
{
  qCDebug(gui_logs) << "TimeSyncPanel::~TimeSyncPanel";
}

void TimeSyncPanel::plot(const QStringList& products, int index)
{
  auto p = new PlotWidget{this};
  addPlot(p, index);
  connect(p, &PlotWidget::parentCreatePlaceHolder, this,
          [this](PlotWidget* caller, SciQLopEnums::Insert insert) {
            int index = this->indexOf(caller);
            if(insert == SciQLopEnums::Insert::below) index += 1;
            this->createPlaceHolder(index);
          });

  connect(p, &PlotWidget::parentDeletePlaceHolder, this,
          [this]() { deletePlaceHolder(); });
  SciQLopCore::pipelines().plot(products, p);
}

bool TimeSyncPanel::deletePlaceHolder()
{
  if(this->placeHolder != nullptr)
  {
    delete this->placeHolder;
    this->placeHolder = nullptr;
  }
  return true;
}

bool TimeSyncPanel::createPlaceHolder(int index)
{
  if(placeHolder == nullptr)
  {
    placeHolder = new PlaceHolder(this);
    placeHolder->setMinimumHeight(height() / (count() + 1));
    connect(
        placeHolder, &PlaceHolder::gotDrop, this,
        [this, index](QDropEvent* event) {
          plot(MIME::ToQStringList(MIME::decode(event->mimeData()->data(
                   MIME::txt(MIME::IDS::PRODUCT_LIST)))),
               index);
          placeHolder = nullptr;
        },
        Qt::DirectConnection);
    connect(placeHolder, &PlaceHolder::destroyed, this,
            [this]() { this->placeHolder = nullptr; });
    dynamic_cast<QVBoxLayout*>(widget()->layout())
        ->insertWidget(index, placeHolder);
    return true;
  }
  return false;
}

DropHelper_default_def(TimeSyncPanel, d_helper);
