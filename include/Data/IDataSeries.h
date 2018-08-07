#ifndef SCIQLOP_IDATASERIES_H
#define SCIQLOP_IDATASERIES_H

#include <Common/MetaTypes.h>
#include <Data/DataSeriesIterator.h>
#include <Data/DateTimeRange.h>
#include <Data/Unit.h>

#include <memory>

#include <QString>

template <int Dim>
class ArrayData;

/**
 * @brief The IDataSeries aims to declare a data series.
 *
 * A data series is an entity that contains at least :
 * - one dataset representing the x-axis
 * - one dataset representing the values
 *
 * Each dataset is represented by an ArrayData, and is associated with a unit.
 *
 * An ArrayData can be unidimensional or two-dimensional, depending on the implementation of the
 * IDataSeries. The x-axis dataset is always unidimensional.
 *
 * @sa ArrayData
 */
class IDataSeries {
public:
    virtual ~IDataSeries() noexcept = default;

    /// Returns the x-axis dataset
    virtual std::shared_ptr<ArrayData<1> > xAxisData() = 0;

    /// Returns the x-axis dataset (as const)
    virtual const std::shared_ptr<ArrayData<1> > xAxisData() const = 0;

    virtual Unit xAxisUnit() const = 0;

    /// @return the y-axis unit, if axis is defined, default unit otherwise
    virtual Unit yAxisUnit() const = 0;

    virtual Unit valuesUnit() const = 0;

    virtual void merge(IDataSeries *dataSeries) = 0;
    /// Removes from data series all entries whose value on the x-axis is not between min and max
    virtual void purge(double min, double max) = 0;

    /// @todo Review the name and signature of this method
    virtual std::shared_ptr<IDataSeries> subDataSeries(const DateTimeRange &range) = 0;

    virtual std::unique_ptr<IDataSeries> clone() const = 0;

    /// @return the total number of points contained in the data series
    virtual int nbPoints() const = 0;

    /// @return the bounds of the y-axis axis (if defined)
    virtual std::pair<double, double> yBounds() const = 0;

    // ///////// //
    // Iterators //
    // ///////// //

    virtual DataSeriesIterator cbegin() const = 0;
    virtual DataSeriesIterator cend() const = 0;
    virtual DataSeriesIterator begin() = 0;
    virtual DataSeriesIterator end() = 0;

    /// @return the iterator to the first entry of the data series whose x-axis data is greater than
    /// or equal to the value passed in parameter, or the end iterator if there is no matching value
    virtual DataSeriesIterator minXAxisData(double minXAxisData) const = 0;

    /// @return the iterator to the last entry of the data series whose x-axis data is less than or
    /// equal to the value passed in parameter, or the end iterator if there is no matching value
    virtual DataSeriesIterator maxXAxisData(double maxXAxisData) const = 0;

    /// @return the iterators pointing to the range of data whose x-axis values are between min and
    /// max passed in parameters
    virtual std::pair<DataSeriesIterator, DataSeriesIterator>
    xAxisRange(double minXAxisData, double maxXAxisData) const = 0;

    /// @return two iterators pointing to the data that have respectively the min and the max value
    /// data of a data series' range. The search is performed for a given x-axis range.
    /// @sa xAxisRange()
    virtual std::pair<DataSeriesIterator, DataSeriesIterator>
    valuesBounds(double minXAxisData, double maxXAxisData) const = 0;

    // /////// //
    // Mutexes //
    // /////// //

    virtual void lockRead() = 0;
    virtual void lockWrite() = 0;
    virtual void unlock() = 0;
};

// Required for using shared_ptr in signals/slots
SCIQLOP_REGISTER_META_TYPE(IDATASERIES_PTR_REGISTRY, std::shared_ptr<IDataSeries>)

#endif // SCIQLOP_IDATASERIES_H
