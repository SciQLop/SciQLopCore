#pragma once
#include "DateTimeRange.h"

/**
 * @brief The DataProviderParameters struct holds the information needed to retrieve data from a
 * data provider
 * @sa IDataProvider
 */
struct DataProviderParameters {
    /// Times for which retrieve data
    DateTimeRange m_Range;
    /// Extra data that can be used by the provider to retrieve data
    QVariantHash m_Data;
};

