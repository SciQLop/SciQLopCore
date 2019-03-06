#ifndef SCIQLOP_VARIABLECACHESTRATEGY_H
#define SCIQLOP_VARIABLECACHESTRATEGY_H

#include "CoreGlobal.h"

#include <Data/DateTimeRange.h>

/// This class aims to hande the cache strategy.
class SCIQLOP_CORE_EXPORT VariableCacheStrategy
{
public:
  virtual ~VariableCacheStrategy() noexcept = default;
  virtual DateTimeRange computeRange(const DateTimeRange& currentCacheRange,
                                     const DateTimeRange& rangeRequested) = 0;
};

#endif // SCIQLOP_VARIABLECACHESTRATEGY_H
