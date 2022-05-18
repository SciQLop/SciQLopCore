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
#include "SciQLopCore/GUI/TimeSyncPannel.hpp"

#include "SciQLopCore/Data/Pipelines.hpp"
#include "SciQLopCore/GUI/PlotWidget.hpp"
#include "SciQLopCore/MimeTypes/MimeTypes.hpp"
#include "SciQLopCore/SciQLopCore.hpp"
#include "SciQLopCore/logging/SciQLopLogs.hpp"
#include "SciQLopPlots/Qt/Graph.hpp"

#include <QDragEnterEvent>
#include <QMimeData>
#include <iostream>

TimeSyncPannel::TimeSyncPannel(QWidget* parent)
    : SciQLopPlots::SyncPannel{parent}, SciQLopObject{this},
      d_helper{{{MIME::IDS::TIME_RANGE,
                 [this](const QMimeData*) {
                   this->setXRange({{}, {}});
                   return true;
                 }},
                {MIME::IDS::PRODUCT_LIST,
                 [this, mime = MIME::txt(MIME::IDS::PRODUCT_LIST)](
                     const QMimeData* data) {
                   this->plot(MIME::ToQStringList(MIME::decode(data->data(mime))));
                   return true;
                 }}}}

{
  setAcceptDrops(true);
  setXRange({(QDateTime::currentSecsSinceEpoch() - 3600 * 24 * 700) * 1.,
             (QDateTime::currentSecsSinceEpoch() - 3600 * 24 * 699) * 1.});
}

TimeSyncPannel::~TimeSyncPannel()
{
  qCDebug(gui_logs) << "TimeSyncPannel::~TimeSyncPannel";
}

void TimeSyncPannel::plot(const QStringList& products)
{
  auto p = new PlotWidget{this};
  addPlot(p);
  SciQLopCore::pipelines().plot(products, p);
}

DropHelper_default_def(TimeSyncPannel,d_helper);
