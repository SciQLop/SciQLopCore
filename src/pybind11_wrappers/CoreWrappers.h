#pragma once
#include "QtWrappers.h"
#include "pywrappers_common.h"

#include <Data/DataSeriesType.h>
#include <Data/DateTimeRange.h>
#include <Data/IDataSeries.h>
#include <Data/Unit.h>
#include <Variable/Variable.h>
#include <Variable/Variable2.h>
#include <pybind11/pybind11.h>
#include <sstream>
#include <string>

PYBIND11_MAKE_OPAQUE(std::vector<double>);

std::ostream& operator<<(std::ostream& os, const Unit& u)
{
  os << "=========================" << std::endl
     << "Unit:" << std::endl
     << " Name: " << u.m_Name << std::endl
     << " Is_TimeUnit: " << u.m_TimeUnit << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const IDataSeries& ds)
{
  os << "=========================" << std::endl
     << "DataSerie:" << std::endl
     << " Number of points:" << ds.nbPoints() << std::endl
     << " X Axis Unit:" << std::endl
     << ds.xAxisUnit() << std::endl
     << " Y Axis Unit:" << std::endl
     << ds.yAxisUnit() << std::endl
     << " Values Axis Unit:" << std::endl
     << ds.valuesUnit() << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const DateTimeRange& range)
{
  os << "=========================" << std::endl
     << "SqpRange:" << std::endl
     << " Start date: " << DateUtils::dateTime(range.m_TStart).toString()
     << std::endl
     << " Stop date: " << DateUtils::dateTime(range.m_TEnd).toString()
     << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Variable& variable)
{
  os << "=========================" << std::endl
     << "Variable:" << std::endl
     << " Name: " << variable.name() << std::endl
     << " range: " << std::endl
     << variable.range() << std::endl
     << " cache range: " << std::endl
     << variable.cacheRange() << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os, const Variable2& variable)
{
  os << "=========================" << std::endl
     << "Variable:" << std::endl
     << " Name: " << variable.name() << std::endl
     << " range: " << std::endl
     << variable.range() << std::endl;
  return os;
}

std::ostream& operator<<(std::ostream& os,
                         const VectorTimeSerie::raw_value_type& value)
{
  os << "[" << value.x << ", " << value.y << ", " << value.z << "]"
     << std::endl;
  return os;
}
