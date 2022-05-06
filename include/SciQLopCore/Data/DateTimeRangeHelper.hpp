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
#include <optional>

#include <cmath>
#include <variant>
#include <QObject>

#include <QDebug>

#include <opaque/numeric_typedef.hpp>
#include <Common/DateUtils.h>
#include <Common/MetaTypes.h>
#include <Numeric.h>
#include <Data/DateTimeRange.h>

enum class TransformationType { ZoomOut, ZoomIn, PanRight, PanLeft, Unknown };

namespace DateTimeRangeHelper {


    inline bool isnan(const DateTimeRange& range)
    {
        return std::isnan(range.m_TStart) && std::isnan(range.m_TEnd);
    }

    inline bool hasnan(const DateTimeRange& range)
    {
        return std::isnan(range.m_TStart) || std::isnan(range.m_TEnd);
    }

    inline bool isPureShift(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        return cpp_utils::numeric::almost_equal<double>(range1.delta(), range2.delta(), 1)
                && !cpp_utils::numeric::almost_equal(range1.m_TStart, range2.m_TStart, 1);
    }

    inline bool isPureZoom(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        return !cpp_utils::numeric::almost_equal<double>(range1.delta(),range2.delta(),1)&&
                cpp_utils::numeric::almost_equal<double>(range1.center(), range2.center(),1);
    }


    /**
     * @brief computeTransformation such as range2 = zoom*range1 + shift
     * @param range1
     * @param range2
     * @return trnaformation applied to range1 to get range2 or an object of type
     * InvalidDateTimeRangeTransformation if the transformation has NaN or forbiden values
     */
    inline std::optional<DateTimeRangeTransformation>
    computeTransformation(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        std::optional<DateTimeRangeTransformation> transformation;
        double zoom = range2.delta()/range1.delta();
        Seconds<double> shift = range2.center() - (range1*zoom).center();
        bool zoomValid = zoom!=0. && !std::isnan(zoom) && !std::isinf(zoom);
        bool shiftValid = !std::isnan(shift.value) && !std::isinf(shift.value);
        if(zoomValid && shiftValid)
            transformation = DateTimeRangeTransformation{zoom, shift};
        return transformation;
    }

    inline TransformationType getTransformationType(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        auto transformation = computeTransformation (range1,range2);
        if(transformation.has_value ())
        {
            if(cpp_utils::numeric::almost_equal(transformation->zoom,1.))
            {
                if(transformation->shift > 0.)
                    return TransformationType::PanRight;
                return TransformationType::PanLeft;
            }
            if(transformation->zoom > 0.)
                return TransformationType::ZoomOut;
            return TransformationType::ZoomIn;
        }
        return TransformationType::Unknown;
    }

}
