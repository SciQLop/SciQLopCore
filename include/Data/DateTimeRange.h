#ifndef SCIQLOP_DATETIMERANGE_H
#define SCIQLOP_DATETIMERANGE_H

#include <cmath>
#include <QObject>

#include <QDebug>

#include <opaque/numeric_typedef.hpp>
#include <Common/DateUtils.h>
#include <Common/MetaTypes.h>
#include <Common/Numeric.h>


template <typename T>
struct Seconds : opaque::numeric_typedef<T, Seconds<T>> ,
        opaque::binop::multipliable     <Seconds<T>, true , Seconds<T>, T, T>,
        opaque::binop::dividable        <Seconds<T>, true , Seconds<T>, T, T>,
        opaque::binop::addable          <Seconds<T>, true , Seconds<T>, T, T>,
        opaque::binop::subtractable     <Seconds<T>, true , Seconds<T>, T, T>

{
  using base  = opaque::numeric_typedef<T, Seconds<T>>;
  using base::base;
  operator T () const {return this->value;}
};

struct InvalidDateTimeRangeTransformation{};

struct DateTimeRangeTransformation
{
    double zoom;
    Seconds<double> shift;
    bool operator==(const DateTimeRangeTransformation& other) const
    {
        return SciQLop::numeric::almost_equal(zoom, other.zoom, 1) &&
                SciQLop::numeric::almost_equal<double>(shift, other.shift, 1);
    }
    DateTimeRangeTransformation merge(const DateTimeRangeTransformation& other) const
    {
        return DateTimeRangeTransformation{zoom*other.zoom,shift+other.shift};
    }
};

/**
 * @brief The SqpRange struct holds the information of time parameters
 */
struct DateTimeRange {
    DateTimeRange()
        :m_TStart(std::nan("")), m_TEnd(std::nan(""))
    {}
    DateTimeRange(double TStart, double TEnd)
        :m_TStart(TStart), m_TEnd(TEnd)
    {}
    /// Creates SqpRange from dates and times
    static DateTimeRange fromDateTime(const QDate &startDate, const QTime &startTime,
                                 const QDate &endDate, const QTime &endTime)
    {
        return {DateUtils::secondsSinceEpoch(QDateTime{startDate, startTime, Qt::UTC}),
                DateUtils::secondsSinceEpoch(QDateTime{endDate, endTime, Qt::UTC})};
    }

    static DateTimeRange fromDateTime(const QDateTime &start,  const QDateTime &end)
    {
        return {DateUtils::secondsSinceEpoch(start),
                DateUtils::secondsSinceEpoch(end)};
    }

    /// Start time (UTC)
    double m_TStart;
    /// End time (UTC)
    double m_TEnd;

    Seconds<double> delta()const noexcept{return Seconds<double>{this->m_TEnd - this->m_TStart};}

    bool contains(const DateTimeRange &dateTime) const noexcept
    {
        return (m_TStart <= dateTime.m_TStart && m_TEnd >= dateTime.m_TEnd);
    }

    Seconds<double> center() const noexcept
    {
        return Seconds<double>((m_TStart + m_TEnd) / 2.);
    }

    bool intersect(const DateTimeRange &dateTime) const noexcept
    {
        return (m_TEnd >= dateTime.m_TStart && m_TStart <= dateTime.m_TEnd);
    }

    inline DateTimeRange transform(const DateTimeRangeTransformation& tr)const noexcept;

    bool operator==(const DateTimeRange &other) const
    {
        return SciQLop::numeric::almost_equal(m_TStart, other.m_TStart, 1) &&
                SciQLop::numeric::almost_equal(m_TEnd, other.m_TEnd, 1);
    }

    bool operator!=(const DateTimeRange &other) const { return !(*this == other); }

    void grow(double factor)noexcept
    {
        double grow_v{delta()*(factor - 1.)/2.};
        m_TStart -= grow_v;
        m_TEnd += grow_v;
    }

    void shrink(double factor)noexcept
    {
        double shrink_v{this->delta()*(1. - factor)/2.};
        m_TStart += shrink_v;
        m_TEnd -= shrink_v;
    }

    DateTimeRange& operator*=(double k)
    {
        this->grow(k);
        return *this;
    }

    DateTimeRange&  operator/=(double k)
    {
        this->shrink(k);
        return *this;
    }

    // compute set difference
    std::vector<DateTimeRange> operator-(const DateTimeRange& other)const
    {
        std::vector<DateTimeRange> result;
        if(std::isnan(other.m_TStart)||std::isnan(other.m_TEnd)||!this->intersect(other))
        {
            result.emplace_back(m_TStart, m_TEnd);
        }
        else
        {
            if(this->m_TStart<other.m_TStart)
            {
                result.emplace_back(this->m_TStart, other.m_TStart);
            }
            if(this->m_TEnd>other.m_TEnd)
            {
                result.emplace_back(other.m_TEnd, this->m_TEnd);
            }
        }
        return result;
    }

};

template <class T>
DateTimeRange& operator+=(DateTimeRange&r, Seconds<T> offset)
{
    shift(r,offset);
    return r;
}

template <class T>
DateTimeRange&  operator-=(DateTimeRange&r, Seconds<T> offset)
{
    shift(r,-offset);
    return r;
}

template <class T>
void shift(DateTimeRange& r, Seconds<T> offset)
{
    r.m_TEnd+=static_cast<double>(offset);
    r.m_TStart+=static_cast<double>(offset);
}

inline DateTimeRange operator*(const DateTimeRange& r, double k)
{
    DateTimeRange result{r};
    result.grow(k);
    return result;
}

inline DateTimeRange operator/(const DateTimeRange& r, double k)
{
    DateTimeRange result{r};
    result.shrink(k);
    return result;
}

template<class T>
DateTimeRange operator+(const DateTimeRange& r, Seconds<T> offset)
{
    DateTimeRange result{r};
    shift(result,offset);
    return result;
}

template<class T>
DateTimeRange operator-(const DateTimeRange& r, Seconds<T> offset)
{
    DateTimeRange result{r};
    shift(result,-offset);
    return result;
}

const auto INVALID_RANGE
    = DateTimeRange{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};

inline QDebug operator<<(QDebug d, DateTimeRange obj)
{
    auto tendDateTimeStart = DateUtils::dateTime(obj.m_TStart);
    auto tendDateTimeEnd = DateUtils::dateTime(obj.m_TEnd);

    d << "ts: " << tendDateTimeStart << " te: " << tendDateTimeEnd;
    return d;
}



DateTimeRange DateTimeRange::transform(const DateTimeRangeTransformation &tr) const noexcept
{
    return DateTimeRange{*this} * tr.zoom + tr.shift;
}

// Required for using shared_ptr in signals/slots
SCIQLOP_REGISTER_META_TYPE(SQPRANGE_REGISTRY, DateTimeRange)

#endif // SCIQLOP_DATETIMERANGE_H
