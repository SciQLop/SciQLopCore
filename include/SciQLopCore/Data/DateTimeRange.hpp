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

#include "../Common/DateUtils.hpp"
#include "../Common/MetaTypes.hpp"
#include "SciQLopCore/MimeTypes/MimeTypes.hpp"
#include "SciQLopPlots/axis_range.hpp"

#include <QDate>
#include <QDebug>
#include <QObject>
#include <QTime>
#include <cmath>
#include <cpp_utils/Numeric.h>
#include <vector>

template<typename T> struct Seconds
{
  inline Seconds(const T value) : value{value} {}
  operator T() const { return this->value; }

  Seconds operator+(const Seconds& other) const
  {
    return Seconds{this->value + other.value};
  }

  Seconds operator-(const Seconds& other) const
  {
    return Seconds{this->value + other.value};
  }

  Seconds operator*(const T factor) const
  {
    return Seconds{this->value * factor};
  }

  Seconds operator/(const T factor) const
  {
    return Seconds{this->value / factor};
  }

  Seconds operator+=(const Seconds& other)
  {
    this->value += other.value;
    return *this;
  }

  Seconds operator-=(const Seconds& other)
  {
    this->value -= other.value;
    return *this;
  }

  Seconds operator*=(const T factor)
  {
    this->value *= factor;
    return *this;
  }

  Seconds operator/=(const T factor)
  {
    this->value /= factor;
    return *this;
  }

private:
  T value;
};

struct InvalidDateTimeRangeTransformation
{};

struct DateTimeRangeTransformation
{
  double zoom;
  Seconds<double> shift;
  bool operator==(const DateTimeRangeTransformation& other) const
  {
    return cpp_utils::numeric::almost_equal(zoom, other.zoom, 1) &&
           cpp_utils::numeric::almost_equal<double>(shift, other.shift, 1);
  }
  DateTimeRangeTransformation
  merge(const DateTimeRangeTransformation& other) const
  {
    return DateTimeRangeTransformation{zoom * other.zoom, shift + other.shift};
  }
};

/**
 * @brief The SqpRange struct holds the information of time parameters
 */
struct DateTimeRange
{
  DateTimeRange() : m_TStart(std::nan("")), m_TEnd(std::nan("")) {}
  DateTimeRange(double TStart, double TEnd) : m_TStart(TStart), m_TEnd(TEnd) {}
  DateTimeRange(const std::pair<double, double>& range)
      : m_TStart(range.first), m_TEnd(range.second)
  {}
  /// Creates SqpRange from dates and times
  static DateTimeRange fromDateTime(const QDate& startDate,
                                    const QTime& startTime,
                                    const QDate& endDate, const QTime& endTime)
  {
    return {
        DateUtils::secondsSinceEpoch(QDateTime{startDate, startTime, Qt::UTC}),
        DateUtils::secondsSinceEpoch(QDateTime{endDate, endTime, Qt::UTC})};
  }

  static DateTimeRange fromDateTime(const QDateTime& start,
                                    const QDateTime& end)
  {
    return {DateUtils::secondsSinceEpoch(start),
            DateUtils::secondsSinceEpoch(end)};
  }

  /// Start time (UTC)
  double m_TStart;
  /// End time (UTC)
  double m_TEnd;

  inline SciQLopPlots::axis::range to_axis_range() const
  {
    return {this->m_TStart, this->m_TEnd};
  }

  Seconds<double> delta() const noexcept
  {
    return Seconds<double>{this->m_TEnd - this->m_TStart};
  }

  bool contains(const DateTimeRange& dateTime) const noexcept
  {
    return (m_TStart <= dateTime.m_TStart && m_TEnd >= dateTime.m_TEnd);
  }

  Seconds<double> center() const noexcept
  {
    return Seconds<double>((m_TStart + m_TEnd) / 2.);
  }

  bool intersect(const DateTimeRange& dateTime) const noexcept
  {
    return (m_TEnd >= dateTime.m_TStart && m_TStart <= dateTime.m_TEnd);
  }

  inline DateTimeRange
  transform(const DateTimeRangeTransformation& tr) const noexcept;

  bool operator==(const DateTimeRange& other) const
  {
    return cpp_utils::numeric::almost_equal(m_TStart, other.m_TStart, 1) &&
           cpp_utils::numeric::almost_equal(m_TEnd, other.m_TEnd, 1);
  }

  bool operator!=(const DateTimeRange& other) const
  {
    return !(*this == other);
  }

  void grow(double factor) noexcept
  {
    double grow_v{delta() * (factor - 1.) / 2.};
    m_TStart -= grow_v;
    m_TEnd += grow_v;
  }

  void shrink(double factor) noexcept
  {
    double shrink_v{this->delta() * (1. - factor) / 2.};
    m_TStart += shrink_v;
    m_TEnd -= shrink_v;
  }

  DateTimeRange& operator*=(double k)
  {
    this->grow(k);
    return *this;
  }

  DateTimeRange& operator/=(double k)
  {
    this->shrink(k);
    return *this;
  }

  // compute set difference
  std::vector<DateTimeRange> operator-(const DateTimeRange& other) const
  {
    std::vector<DateTimeRange> result;
    if(std::isnan(other.m_TStart) || std::isnan(other.m_TEnd) ||
       !this->intersect(other))
    {
      result.emplace_back(m_TStart, m_TEnd);
    }
    else
    {
      if(this->m_TStart < other.m_TStart)
      {
        result.emplace_back(this->m_TStart, other.m_TStart);
      }
      if(this->m_TEnd > other.m_TEnd)
      {
        result.emplace_back(other.m_TEnd, this->m_TEnd);
      }
    }
    return result;
  }
};

template<class T> DateTimeRange& operator+=(DateTimeRange& r, Seconds<T> offset)
{
  shift(r, offset);
  return r;
}

template<class T> DateTimeRange& operator-=(DateTimeRange& r, Seconds<T> offset)
{
  shift(r, -offset);
  return r;
}

template<class T> void shift(DateTimeRange& r, Seconds<T> offset)
{
  r.m_TEnd += static_cast<double>(offset);
  r.m_TStart += static_cast<double>(offset);
}

inline DateTimeRange operator*(const DateTimeRange& r, double k)
{
  DateTimeRange result{r};
  result.grow(k);
  return result;
}

inline DateTimeRange operator/(const DateTimeRange& r, double k)
{
  DateTimeRange result{r};
  result.shrink(k);
  return result;
}

template<class T>
DateTimeRange operator+(const DateTimeRange& r, Seconds<T> offset)
{
  DateTimeRange result{r};
  shift(result, offset);
  return result;
}

template<class T>
DateTimeRange operator-(const DateTimeRange& r, Seconds<T> offset)
{
  DateTimeRange result{r};
  shift(result, -offset);
  return result;
}

const auto INVALID_RANGE =
    DateTimeRange{std::numeric_limits<double>::quiet_NaN(),
                  std::numeric_limits<double>::quiet_NaN()};

inline QDebug operator<<(QDebug d, DateTimeRange obj)
{
  auto tendDateTimeStart = DateUtils::dateTime(obj.m_TStart);
  auto tendDateTimeEnd   = DateUtils::dateTime(obj.m_TEnd);

  d << "ts: " << tendDateTimeStart << " te: " << tendDateTimeEnd;
  return d;
}

DateTimeRange
DateTimeRange::transform(const DateTimeRangeTransformation& tr) const noexcept
{
  return DateTimeRange{*this} * tr.zoom + tr.shift;
}

// Required for using shared_ptr in signals/slots
SCIQLOP_REGISTER_META_TYPE(SQPRANGE_REGISTRY, DateTimeRange)

namespace MIME
{
  template<> inline QMimeData* mimeData<DateTimeRange>(const DateTimeRange& dt)
  {
    QMimeData* data = new QMimeData;
    data->setData(MIME::MIME_TYPE_TIME_RANGE,
                  MIME::encode(QVariantList{dt.m_TStart, dt.m_TEnd}));
    return data;
  }

  template<typename T>
  inline std::enable_if_t<std::is_same_v<T, DateTimeRange>, T>
  mimeDataTo(const QMimeData* data)
  {
    if(data != nullptr)
    {
      auto v = MIME::decode(data->data(MIME::MIME_TYPE_TIME_RANGE));
      if((v.length() == 2) && (v[0].userType() == QMetaType::Double) &&
         (v[1].userType() == QMetaType::Double))
      {
        return {v[0].toDouble(), v[1].toDouble()};
      }
    }
    return {std::nan(""), std::nan("")};
  }

  template<typename T>
  inline std::enable_if_t<std::is_same_v<T, SciQLopPlots::axis::range>, T>
  mimeDataTo(const QMimeData* data)
  {
    if(data != nullptr)
    {
      auto v = MIME::decode(data->data(MIME::MIME_TYPE_TIME_RANGE));
      if((v.length() == 2) && (v[0].userType() == QMetaType::Double) &&
         (v[1].userType() == QMetaType::Double))
      {
        return {v[0].toDouble(), v[1].toDouble()};
      }
    }
    return {std::nan(""), std::nan("")};
  }

}; // namespace MIME
