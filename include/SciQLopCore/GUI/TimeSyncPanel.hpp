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

#include <QDockWidget>
#include <QWidget>
#include <SciQLopPlots/Qt/SyncPanel.hpp>

#include "SciQLopCore/Common/SciQLopObject.hpp"
#include "SciQLopCore/GUI/DragAndDrop.hpp"

class EventTimeSpan;
class PlotWidget;

class TimeSyncPanel : public SciQLopPlots::SyncPanel, public SciQLopObject
{
  Q_OBJECT
  DropHelper d_helper;
  PlaceHolder* placeHolder=nullptr;
public:
  TimeSyncPanel(QWidget* parent = nullptr);
  virtual ~TimeSyncPanel() override;

  void plot(const QStringList& products, int index=-1);
  bool deletePlaceHolder();

  void setTimeRange(double start, double stop);
  void autoScaleY();

  friend EventTimeSpan;

  Q_SIGNAL void plotAdded(PlotWidget*);

private:
  bool createPlaceHolder(int index);

  QList<SciQLopPlots::interfaces::IPlotWidget*> plots() const;
protected:
  DropHelper_default_decl();

};
