#ifndef SCIQLOP_SQPRANGE_H
#define SCIQLOP_SQPRANGE_H

#include <QObject>

#include <QDebug>

#include <Common/DateUtils.h>
#include <Common/MetaTypes.h>
#include <opaque/numeric_typedef.hpp>

#include <cmath>


template <typename T>
struct Seconds : opaque::numeric_typedef<T, Seconds<T>> ,
        opaque::binop::multipliable     <Seconds<T>, true , Seconds<T>, T, T>,
        opaque::binop::dividable        <Seconds<T>, true , Seconds<T>, T, T>,
        opaque::binop::addable          <Seconds<T>, true , Seconds<T>, T, T>,
        opaque::binop::subtractable     <Seconds<T>, true , Seconds<T>, T, T>

{
  using base  = opaque::numeric_typedef<T, Seconds<T>>;
  using base::base;
};

/**
 * @brief The SqpRange struct holds the information of time parameters
 */
struct DateTimeRange {
    /// Creates SqpRange from dates and times
    static DateTimeRange fromDateTime(const QDate &startDate, const QTime &startTime,
                                 const QDate &endDate, const QTime &endTime)
    {
        return {DateUtils::secondsSinceEpoch(QDateTime{startDate, startTime, Qt::UTC}),
                DateUtils::secondsSinceEpoch(QDateTime{endDate, endTime, Qt::UTC})};
    }

    /// Start time (UTC)
    double m_TStart;
    /// End time (UTC)
    double m_TEnd;

    Seconds<double> delta()const {return Seconds<double>{this->m_TEnd - this->m_TStart};}

    bool contains(const DateTimeRange &dateTime) const noexcept
    {
        return (m_TStart <= dateTime.m_TStart && m_TEnd >= dateTime.m_TEnd);
    }

    bool intersect(const DateTimeRange &dateTime) const noexcept
    {
        return (m_TEnd >= dateTime.m_TStart && m_TStart <= dateTime.m_TEnd);
    }

    bool operator==(const DateTimeRange &other) const
    {
        auto equals = [](const auto &v1, const auto &v2) {
            return (std::isnan(v1) && std::isnan(v2)) || v1 == v2;
        };

        return equals(m_TStart, other.m_TStart) && equals(m_TEnd, other.m_TEnd);
    }
    bool operator!=(const DateTimeRange &other) const { return !(*this == other); }

    void grow(double factor)
    {
        double grow_v{delta()*(factor - 1.)/2.};
        m_TStart -= grow_v;
        m_TEnd += grow_v;
    }

    void shrink(double factor)
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

// Required for using shared_ptr in signals/slots
SCIQLOP_REGISTER_META_TYPE(SQPRANGE_REGISTRY, DateTimeRange)

#endif // SCIQLOP_SQPRANGE_H
