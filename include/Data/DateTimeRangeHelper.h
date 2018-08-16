#ifndef SCIQLOP_DATETIMERANGEHELPER_H
#define SCIQLOP_DATETIMERANGEHELPER_H

#include <optional>

#include <cmath>
#include <variant>
#include <QObject>

#include <QDebug>

#include <opaque/numeric_typedef.hpp>
#include <Common/DateUtils.h>
#include <Common/MetaTypes.h>
#include <Common/Numeric.h>
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
        return SciQLop::numeric::almost_equal<double>(range1.delta(), range2.delta(), 1)
                && !SciQLop::numeric::almost_equal(range1.m_TStart, range2.m_TStart, 1);
    }

    inline bool isPureZoom(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        return !SciQLop::numeric::almost_equal<double>(range1.delta(),range2.delta(),1)&&
                SciQLop::numeric::almost_equal<double>(range1.center(), range2.center(),1);
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
            if(SciQLop::numeric::almost_equal(transformation->zoom,1.))
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

#endif // SCIQLOP_DATETIMERANGEHELPER_H
