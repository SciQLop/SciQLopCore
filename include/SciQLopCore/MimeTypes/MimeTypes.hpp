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

#include <QByteArray>
#include <QDataStream>
#include <QMimeData>
#include <QString>
#include <QVariant>
#include <QIODevice>

// ////////////////// //
// SciQlop Mime Types //
// ////////////////// //

namespace MIME
{
  inline QString const MIME_TYPE_GRAPH =
      QStringLiteral("application/x.sciqlop.graph");
  inline QString const MIME_TYPE_ZONE =
      QStringLiteral("application/x.sciqlop.zone");
  inline QString const MIME_TYPE_VARIABLE_LIST =
      QStringLiteral("application/x.sciqlop.var-list");
  inline QString const MIME_TYPE_PRODUCT_LIST =
      QStringLiteral("application/x.sciqlop.product-list");
  inline QString const MIME_TYPE_TIME_RANGE =
      QStringLiteral("application/x.sciqlop.time-range");
  inline QString const MIME_TYPE_SELECTION_ZONE =
      QStringLiteral("application/x.sciqlop.selection-zone");
  inline QString const MIME_TYPE_EVENT_LIST =
      QStringLiteral("application/x.sciqlop.event-list");
  inline QString const MIME_TYPE_SOURCE_CATALOGUE_LIST =
      QStringLiteral("application/x.sciqlop.source-catalogue-list");

  enum class IDS
  {
    GRAPH,
    ZONE,
    VARIABLE_LIST,
    PRODUCT_LIST,
    TIME_RANGE,
    SELECTION_ZONE,
    EVENT_LIST,
    SOURCE_CATALOGUE_LIST
  };

  inline std::unordered_map<IDS, QString> const MIME_TYPES = {
      {IDS::GRAPH, MIME_TYPE_GRAPH},
      {IDS::ZONE, MIME_TYPE_ZONE},
      {IDS::VARIABLE_LIST, MIME_TYPE_VARIABLE_LIST},
      {IDS::PRODUCT_LIST, MIME_TYPE_PRODUCT_LIST},
      {IDS::TIME_RANGE, MIME_TYPE_TIME_RANGE},
      {IDS::SELECTION_ZONE, MIME_TYPE_SELECTION_ZONE},
      {IDS::EVENT_LIST, MIME_TYPE_EVENT_LIST},
      {IDS::SOURCE_CATALOGUE_LIST, MIME_TYPE_SOURCE_CATALOGUE_LIST}};

  inline QVariantList decode(const QByteArray& mimeData)
  {
    QDataStream stream{mimeData};
    QVariantList productList;
    stream >> productList;
    return productList;
  }

  inline QByteArray encode(const QVariantList& data)
  {
    QByteArray encodedData;
    QDataStream stream{&encodedData, QIODevice::WriteOnly};
    stream << data;
    return encodedData;
  }

  inline QByteArray encode(const QString& data)
  {
    QByteArray encodedData;
    QDataStream stream{&encodedData, QIODevice::WriteOnly};
    stream << (QVariantList{} << QVariant{data});
    return encodedData;
  }

  // override mimeData in corresponding object source code
  template<typename T> QMimeData* mimeData(const T& object) = delete;

  template<IDS id> QVariantList fromMimeData(const QMimeData* mime)
  {
    return decode(mime->data(MIME_TYPES.at(id)));
  }

} // namespace MIME
