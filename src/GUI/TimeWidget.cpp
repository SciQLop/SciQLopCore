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
#include "SciQLopCore/GUI/TimeWidget.hpp"
#include "ui_TimeWidget.h"

#include "SciQLopCore/Common/DateUtils.hpp"
#include "SciQLopCore/MimeTypes/MimeTypes.hpp"

#include <QMimeData>
#include <QStyle>
#include <QMouseEvent>
#include <QDrag>

TimeWidget::TimeWidget(QWidget* parent)
        : QWidget { parent }
        , ui { new Ui::TimeWidget }
{
    ui->setupUi(this);

 //   connect(ui->startDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this,
 //       &TimeWidget::onTimeUpdateRequested);

 //   connect(ui->endDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this,
 //       &TimeWidget::onTimeUpdateRequested);

    auto endDateTime = QDateTime::currentDateTimeUtc();
    auto startDateTime = endDateTime.addSecs(-3600); // one hour before

    ui->startDateTimeEdit->setDateTime(startDateTime);
    ui->endDateTimeEdit->setDateTime(endDateTime);

}


TimeWidget::~TimeWidget()
{
    delete ui;
}

void TimeWidget::setTimeRange(DateTimeRange time)
{
    auto startDateTime = DateUtils::dateTime(time.m_TStart);
    auto endDateTime = DateUtils::dateTime(time.m_TEnd);

    ui->startDateTimeEdit->setDateTime(startDateTime);
    ui->endDateTimeEdit->setDateTime(endDateTime);
}

DateTimeRange TimeWidget::timeRange() const
{
    return DateTimeRange { DateUtils::secondsSinceEpoch(ui->startDateTimeEdit->dateTime()),
        DateUtils::secondsSinceEpoch(ui->endDateTimeEdit->dateTime()) };
}

void TimeWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {

    QDrag *drag = new QDrag(this);
    drag->setMimeData(MIME::mimeData(timeRange()));
    drag->setPixmap(QPixmap{"://icons/time.png"}.scaledToHeight(32));

    Qt::DropAction dropAction = drag->exec();

  }
}

