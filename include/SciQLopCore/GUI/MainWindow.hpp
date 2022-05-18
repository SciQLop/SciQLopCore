/*------------------------------------------------------------------------------
-- This file is a part of the SciQLop Software
-- Copyright (C) 2017, Plasma Physics Laboratory - CNRS
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

#include <QMainWindow>
#include <QWidget>

namespace Ui
{
class MainWindow;
} // Ui
class TimeSyncPanel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  virtual ~MainWindow() override;

  void addTimeSyncPannel(TimeSyncPanel* pannel);
  void addWidgetIntoDock(Qt::DockWidgetArea,QWidget*);

protected:
  void changeEvent(QEvent* e) override;
  void closeEvent(QCloseEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override;

private:
  Ui::MainWindow* ui;
};

inline void init_resources()
{
  // Q_IMPORT_PLUGIN(DemoPlugin);
  // Q_INIT_RESOURCE(sqpguiresources);
 // SqpApplication::setOrganizationName("LPP");
 // SqpApplication::setOrganizationDomain("lpp.fr");
  //SqpApplication::setApplicationName("SciQLop");
}


