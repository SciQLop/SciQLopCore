#ifndef SCIQLOP_VARIABLECACHESTRATEGYFACTORY_H
#define SCIQLOP_VARIABLECACHESTRATEGYFACTORY_H


#include <memory>
#include <stdexcept>

#include "VariableCacheStrategy.h"
#include "ProportionalCacheStrategy.h"
#include "SingleThresholdCacheStrategy.h"

#include <Common/debug.h>
#include <QString>


enum class CacheStrategy { Proportional, SingleThreshold, TwoThreshold };

class VariableCacheStrategyFactory {

    using cacheStratPtr = std::unique_ptr<VariableCacheStrategy>;

public:
    static cacheStratPtr createCacheStrategy(CacheStrategy specificStrategy)
    {
        switch (specificStrategy) {
        case CacheStrategy::Proportional: {
            return std::unique_ptr<VariableCacheStrategy>{
                new ProportionalCacheStrategy{}};
        }
        case CacheStrategy::SingleThreshold: {
            return std::unique_ptr<VariableCacheStrategy>{
                new SingleThresholdCacheStrategy{}};
        }
        case CacheStrategy::TwoThreshold: {
            SCIQLOP_ERROR(VariableCacheStrategyFactory, "CacheStrategy::TwoThreshold not implemented yet");
            break;
        }
        default:
            SCIQLOP_ERROR(VariableCacheStrategyFactory, "Unknown cache strategy");
            break;
        }

        return nullptr;
    }
};


#endif // VARIABLECACHESTRATEGYFACTORY_H
