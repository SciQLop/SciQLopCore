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
#include "SciQLopCore/GUI/PlotWidget.hpp"

#include "SciQLopCore/Data/Pipelines.hpp"
#include "SciQLopCore/DataSource/DataSources.hpp"
#include "SciQLopCore/GUI/TimeSyncPannel.hpp"
#include "SciQLopCore/SciQLopCore.hpp"
#include "SciQLopCore/logging/SciQLopLogs.hpp"

PlotWidget::PlotWidget(QWidget* parent)
    : SciQLopPlots::SciQLopPlot{parent}, SciQLopObject{this},
      d_helper{
          {{MIME::IDS::TIME_RANGE,
            [this](const QMimeData*) {
              this->setXRange({{}, {}});
              return true;
            }},
           {MIME::IDS::PRODUCT_LIST, [mime = MIME::txt(MIME::IDS::PRODUCT_LIST),
                                      this](const QMimeData* data) {
              if(!parentHasPlaceHolder)
              {
                this->plot(MIME::ToQStringList(MIME::decode(data->data(mime))));
              }
              return true;
            }}}}
{
  this->setAcceptDrops(true);
}

void PlotWidget::plot(const QStringList& products)
{
  SciQLopCore::pipelines().plot(products, this);
}

bool PlotWidget::createPlaceHolder(const QPointF& position)
{
  qCDebug(gui_logs) << "PlotWidget::createPlaceHolder";
  const auto y = position.y();
  if((0 < y) && (y < (0.2 * height())))
  {
    emit parentCreatePlaceHolder(this, SciQLopEnums::Insert::above);
    parentHasPlaceHolder = true;
  }
  else if(((0.8 * height()) < y) && (y < height()))
  {
    emit parentCreatePlaceHolder(this, SciQLopEnums::Insert::below);
    parentHasPlaceHolder = true;
  }
  return parentHasPlaceHolder;
}

DropHelper_default_def(PlotWidget, d_helper)
