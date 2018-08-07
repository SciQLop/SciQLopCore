#ifndef SCIQLOP_VARIABLECACHECONTROLLER_H
#define SCIQLOP_VARIABLECACHECONTROLLER_H

#include "CoreGlobal.h"

#include <QLoggingCategory>
#include <QObject>

#include <Data/DateTimeRange.h>

#include <QLoggingCategory>

#include <Common/spimpl.h>

Q_DECLARE_LOGGING_CATEGORY(LOG_VariableCacheController)

class Variable;

/// This class aims to store in the cache all of the dateTime already requested to the variable.
class SCIQLOP_CORE_EXPORT VariableCacheController : public QObject {
    Q_OBJECT
public:
    explicit VariableCacheController(QObject *parent = 0);


    void addDateTime(std::shared_ptr<Variable> variable, const DateTimeRange &dateTime);

    /// Clears cache concerning a variable
    void clear(std::shared_ptr<Variable> variable) noexcept;

    /// Return all of the SqpDataTime part of the dateTime whose are not in the cache
    QVector<DateTimeRange> provideNotInCacheDateTimeList(std::shared_ptr<Variable> variable,
                                                    const DateTimeRange &dateTime);


    QVector<DateTimeRange> dateCacheList(std::shared_ptr<Variable> variable) const noexcept;

    void displayCache(std::shared_ptr<Variable> variable) const;

private:
    class VariableCacheControllerPrivate;
    spimpl::unique_impl_ptr<VariableCacheControllerPrivate> impl;
};

#endif // SCIQLOP_VARIABLECACHECONTROLLER_H
