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
#include "SciQLopCore/GUI/MainWindow.hpp"

#include "SciQLopCore/GUI/TimeSyncPanel.hpp"
#include "SciQLopCore/GUI/TimeWidget.hpp"
#include "ui_mainwindow.h"

#include <QAction>
#include <QCloseEvent>
#include <QDate>
#include <QDir>
#include <QFileDialog>
#include <QLoggingCategory>
#include <QMessageBox>
#include <QToolBar>
#include <QToolButton>

inline void init_resources()
{
#ifdef SCIQLOP_STATIC_BUILD
  Q_INIT_RESOURCE(sqpguiresources);
#endif
}

Q_LOGGING_CATEGORY(LOG_MainWindow, "MainWindow")

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow{parent}, ui{new Ui::MainWindow}
{
  init_resources();
  ui->setupUi(this);
  ui->addTSPannel->setIcon(QIcon{"://icons/add.png"});
  ui->toolBar->insertAction(ui->addTSPannel, new TimeWidgetAction);
  setWindowIcon(QIcon{"://icons/SciQLop.png"});
  connect(ui->addTSPannel, &QAction::triggered, this,
          [this]() { this->addTimeSyncPannel(new TimeSyncPanel); });

  connect(ui->centralwidget,&CentralWidget::panels_list_changed,this,&MainWindow::panels_list_changed);
}

MainWindow::~MainWindow() {}

void MainWindow::plot(const QStringList &products)
{
    ui->centralwidget->plot(products);
}

void MainWindow::addTimeSyncPannel(TimeSyncPanel* pannel)
{
  this->ui->centralwidget->addTimeSyncPanel(pannel);
}

void MainWindow::addWidgetIntoDock(Qt::DockWidgetArea area, QWidget* w)
{
  if(w)
  {
    auto doc = new QDockWidget(this);
    doc->setAllowedAreas(Qt::AllDockWidgetAreas);
    doc->setWidget(w);
    this->addDockWidget(area, doc);
    if(auto so = dynamic_cast<SciQLopObject*>(w); so)
    {
      doc->setWindowTitle(so->name());
    }
  }
}

QToolBar* MainWindow::toolBar() { return this->ui->toolBar; }

TimeSyncPanel *MainWindow::plotPanel(const QString &name)
{
    return this->ui->centralwidget->plotPanel(name);
}

QStringList MainWindow::panels() const
{
    return this->ui->centralwidget->panels();
}

void MainWindow::changeEvent(QEvent* e)
{
  QMainWindow::changeEvent(e);
  switch(e->type())
  {
    case QEvent::LanguageChange: break;
    default: break;
  }
}

void MainWindow::closeEvent(QCloseEvent* event) { event->accept(); }

void MainWindow::keyPressEvent(QKeyEvent* event)
{
  switch(event->key())
  {
    case Qt::Key_F11:
      if(this->isFullScreen()) { this->showNormal(); }
      else { this->showFullScreen(); }
      break;
    default: break;
  }
}
