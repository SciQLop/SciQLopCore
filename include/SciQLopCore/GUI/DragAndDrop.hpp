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
#include "SciQLopCore/Common/SciQLopObject.hpp"
#include "SciQLopCore/MimeTypes/MimeTypes.hpp"
#include "SciQLopCore/logging/SciQLopLogs.hpp"

#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMainWindow>
#include <QMimeData>
#include <QTabBar>
#include <cpp_utils/containers/algorithms.hpp>
#include <functional>
#include <tuple>

struct DropHandler
{
  MIME::IDS mime_id;
  QString mime_str;
  std::function<bool(const QMimeData*)> callback;

  DropHandler(MIME::IDS id, std::function<bool(const QMimeData*)>&& callback)
      : mime_id{id}, mime_str{MIME::txt(id)}, callback(std::move(callback))
  {}
  DropHandler(MIME::IDS id,
              const std::function<bool(const QMimeData*)>& callback)
      : mime_id{id}, mime_str{MIME::txt(id)}, callback(callback)
  {}
};

struct DropHelper
{
  inline DropHelper(std::initializer_list<DropHandler> handlers)
      : drop_handlers{handlers}
  {}

  template<typename Widget_t>
  inline void dragEnterEvent(Widget_t* self, QDragEnterEvent* event)
  {
    const auto& formats = event->mimeData()->formats();
    qCDebug(gui_logs) << SciQLopObject::className(self)
                      << "dragEnterEvent: " << formats;
    for(auto i = 0UL; i < std::size(drop_handlers); i++)
    {
      if(cpp_utils::containers::contains(formats, drop_handlers[i].mime_str))
      {
        current_handler_index = i;
        event->acceptProposedAction();
        return;
      }
    }
    if constexpr(std::is_base_of_v<QMainWindow, Widget_t>)
    {
      event->acceptProposedAction();
    }
    current_handler_index = -1;
  }

  template<typename Widget_t>
  inline void dragMoveEvent(Widget_t* self, QDragMoveEvent* event)
  {
    if(current_handler_index != -1)
      event->acceptProposedAction();
    else
    {
      if constexpr(std::is_base_of_v<QMainWindow, Widget_t>)
      {
        qCDebug(gui_logs) << "dragMoveEvent is QMainWindow";
        QTabBar* tabBar = dynamic_cast<QTabBar*>(self->childAt(event->position().toPoint()));
        if(tabBar != nullptr)
        {
          tabBar->setCurrentIndex(tabBar->tabAt(tabBar->mapFromParent(event->position().toPoint())));
        }
      }
    }
  }

  template<typename Widget_t>
  inline void dragLeaveEvent(Widget_t* self, QDragLeaveEvent* event)
  {}
  template<typename Widget_t>
  inline void dropEvent(Widget_t* self, QDropEvent* event)
  {
    if(current_handler_index != -1)
    {
      if(drop_handlers[current_handler_index].callback(event->mimeData()))
      {
        event->acceptProposedAction();
        event->accept();
        qCDebug(gui_logs)
            << SciQLopObject::className(self) << "dropEvent: "
            << MIME::decode(event->mimeData()->data(
                   drop_handlers[current_handler_index].mime_str));
      }
    }
  }

private:
  std::vector<DropHandler> drop_handlers;
  int current_handler_index = -1;
};

#define DropHelper_default_decl()                                              \
  virtual void dragEnterEvent(QDragEnterEvent* event) override;                \
  virtual void dragMoveEvent(QDragMoveEvent* event) override;                  \
  virtual void dragLeaveEvent(QDragLeaveEvent* event) override;                \
  virtual void dropEvent(QDropEvent* event) override

#define DropHelper_default_def(classname, d_helper)                            \
  void classname::dragEnterEvent(QDragEnterEvent* event)                       \
  {                                                                            \
    d_helper.dragEnterEvent(this, event);                                      \
  }                                                                            \
  void classname::dragMoveEvent(QDragMoveEvent* event)                         \
  {                                                                            \
    d_helper.dragMoveEvent(this, event);                                       \
  }                                                                            \
  void classname::dragLeaveEvent(QDragLeaveEvent* event)                       \
  {                                                                            \
    d_helper.dragLeaveEvent(this, event);                                      \
  }                                                                            \
  void classname::dropEvent(QDropEvent* event)                                 \
  {                                                                            \
    d_helper.dropEvent(this, event);                                           \
  }
