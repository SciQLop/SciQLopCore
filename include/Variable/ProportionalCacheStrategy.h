#ifndef SCIQLOP_VARIABLESINGLETHRESHOLDCACHESTRATEGY_H
#define SCIQLOP_VARIABLESINGLETHRESHOLDCACHESTRATEGY_H

#include "Settings/SqpSettingsDefs.h"
#include "VariableCacheStrategy.h"


/// This class aims to hande the cache strategy.
class SCIQLOP_CORE_EXPORT ProportionalCacheStrategy : public VariableCacheStrategy {
public:
    ProportionalCacheStrategy() = default;

    DateTimeRange computeRange(const DateTimeRange &currentCacheRange,
                                               const DateTimeRange &rangeRequested) override
    {
        Q_UNUSED(currentCacheRange);
        auto toleranceFactor = SqpSettings::toleranceValue(
            GENERAL_TOLERANCE_AT_UPDATE_KEY, GENERAL_TOLERANCE_AT_UPDATE_DEFAULT_VALUE);
        auto tolerance = toleranceFactor * (rangeRequested.m_TEnd - rangeRequested.m_TStart);

        return  rangeRequested*(1.+tolerance);

    }
};


#endif // SCIQLOP_VARIABLESINGLETHRESHOLDCACHESTRATEGY_H
