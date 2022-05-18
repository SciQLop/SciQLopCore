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

#include "SciQLopCore/SciQLopCore.hpp"
#include "SciQLopCore/Common/SciQLopObject.hpp"
#include "SciQLopCore/GUI/DragAndDrop.hpp"

#include <QWidget>
#include <SciQLopPlots/Qt/QCustomPlot/SciQLopPlots.hpp>



class PlotWidget : public SciQLopPlots::SciQLopPlot, public SciQLopObject
{
  Q_OBJECT

    DropHelper d_helper;
    bool parentHasPlaceHolder=false;
public:
  PlotWidget(QWidget* parent);

  void plot(const QStringList& products);

  bool createPlaceHolder(const QPointF& position);
  bool deletePlaceHolder();

  Q_SIGNAL void parentCreatePlaceHolder(PlotWidget* caller,SciQLopEnums::Insert insert);
  Q_SIGNAL void parentDeletePlaceHolder();
protected:
  DropHelper_default_decl();
};
