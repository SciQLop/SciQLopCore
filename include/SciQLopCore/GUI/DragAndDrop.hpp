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
#include "SciQLopCore/MimeTypes/MimeTypes.hpp"
#include "SciQLopCore/logging/SciQLopLogs.hpp"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
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

template<typename Widget_t> struct DropHelper : Widget_t
{
  inline DropHelper(QWidget* parent,
                    std::initializer_list<DropHandler> handlers)
      : Widget_t{parent}, drop_handlers{handlers}
  {
  }

protected:
  virtual inline void dragEnterEvent(QDragEnterEvent* event) override
  {
    const auto& formats = event->mimeData()->formats();
    for(int i = 0; i < std::size(drop_handlers); i++)
    {
      if(cpp_utils::containers::contains(formats, drop_handlers[i].mime_str))
      {
        current_handler_index = i;
        event->acceptProposedAction();
        return;
      }
    }
    current_handler_index = -1;
  }

  virtual inline void dragMoveEvent(QDragMoveEvent* event) override
  {
    if(current_handler_index) event->acceptProposedAction();
  }
  virtual inline void dragLeaveEvent(QDragLeaveEvent* event) override {}
  virtual inline void dropEvent(QDropEvent* event) override
  {
    if(current_handler_index != -1)
    {
      if(drop_handlers[current_handler_index].callback(event->mimeData()))
      {
        event->acceptProposedAction();
        event->accept();
        qCDebug(gui_logs)
            << "dropEvent: "
            << MIME::decode(event->mimeData()->data(
                   drop_handlers[current_handler_index].mime_str));
      }
    }
  }

private:
  std::vector<DropHandler> drop_handlers;
  int current_handler_index = -1;
};
