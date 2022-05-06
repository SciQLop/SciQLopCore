#pragma once

#include <QDateTime>

/// Format for datetimes
const auto DATETIME_FORMAT = QStringLiteral("dd/MM/yyyy \nhh:mm:ss:zzz");
const auto DATETIME_FORMAT_ONE_LINE = QStringLiteral("dd/MM/yyyy hh:mm:ss:zzz");

/**
 * Utility class with methods for dates
 */
struct DateUtils {
    /// Converts seconds (since epoch) to datetime. By default, the datetime is in UTC
    static QDateTime dateTime(double secs, Qt::TimeSpec timeSpec = Qt::UTC) noexcept;

    /// Converts datetime to seconds since epoch
    static double secondsSinceEpoch(const QDateTime &dateTime) noexcept;
};

