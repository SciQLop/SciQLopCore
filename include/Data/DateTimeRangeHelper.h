#ifndef SCIQLOP_DATETIMERANGEHELPER_H
#define SCIQLOP_DATETIMERANGEHELPER_H

#include <cmath>
#include <QObject>

#include <QDebug>

#include <opaque/numeric_typedef.hpp>
#include <Common/DateUtils.h>
#include <Common/MetaTypes.h>
#include <Common/Numeric.h>
#include <Data/DateTimeRange.h>

namespace DateTimeRangeHelper {



    bool isPureShift(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        return SciQLop::numeric::almost_equal<double>(range1.delta(), range2.delta(), 1.)
                && !SciQLop::numeric::almost_equal(range1.m_TStart, range2.m_TStart, 1.);
    }

    bool isPureZoom(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        return !SciQLop::numeric::almost_equal<double>(range1.delta(),range2.delta(),1.)&&
                SciQLop::numeric::almost_equal<double>(range1.center(), range2.center(),1.);
    }

    /**
     * @brief computeTransformation such as range2 = zoom*range1 + shift
     * @param range1
     * @param range2
     * @return trnaformation applied to range1 to get range2
     */
    DateTimeRangeTransformation computeTransformation(const DateTimeRange& range1, const DateTimeRange& range2)
    {
        DateTimeRangeTransformation transformation;
        transformation.zoom = range2.delta()/range1.delta();
        transformation.shift = range2.center() - (range1*transformation.zoom).center();
        return transformation;
    }

}

#endif // SCIQLOP_DATETIMERANGEHELPER_H
