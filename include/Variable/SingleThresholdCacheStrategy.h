#ifndef SCIQLOP_SINGLETHRESHOLDCACHESTRATEGY_H
#define SCIQLOP_SINGLETHRESHOLDCACHESTRATEGY_H

#include "Settings/SqpSettingsDefs.h"
#include "VariableCacheStrategy.h"

class SCIQLOP_CORE_EXPORT SingleThresholdCacheStrategy : public VariableCacheStrategy {
public:
    SingleThresholdCacheStrategy() = default;

    DateTimeRange computeRange(const DateTimeRange &currentCacheRange,
                                               const DateTimeRange &rangeRequested) override
    {
        Q_UNUSED(currentCacheRange);
        if(currentCacheRange.contains (rangeRequested*1.1))
            return  currentCacheRange;
        return rangeRequested*2.;
    }
};


#endif // SCIQLOP_SINGLETHRESHOLDCACHESTRATEGY_H
