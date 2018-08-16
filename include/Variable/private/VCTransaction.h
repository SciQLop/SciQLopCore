#include <QThreadPool>
#include <QRunnable>
#include <QObject>
#include <QReadWriteLock>

#include "Variable/VariableSynchronizationGroup2.h"
#include <Variable/Variable.h>
#include <Common/containers.h>
#include <Common/debug.h>
#include <Data/DataProviderParameters.h>
#include <Data/DateTimeRangeHelper.h>
#include <Data/DateTimeRange.h>
#include <Data/IDataProvider.h>

struct VCTransaction
{
    VCTransaction(QUuid refVar, DateTimeRange range, int varCount)
        :refVar{refVar},range{range},_remainingVars{varCount}
    {}

    QUuid refVar;
    DateTimeRange range;
    bool ready()
    {
        QReadLocker lock{&_lock};
        return _remainingVars == 0;
    }

    bool done()
    {
        QWriteLocker lock{&_lock};
        _remainingVars-=1;
        return _remainingVars == 0;
    }
private:
    QReadWriteLock _lock;
    int _remainingVars;
};

class TransactionExe:public QObject,public QRunnable
{
    Q_OBJECT
    std::shared_ptr<Variable> _variable;
    std::shared_ptr<IDataProvider> _provider;
    std::vector<DateTimeRange> _ranges;
    DateTimeRange _range;
    DateTimeRange _cacheRange;
public:
    TransactionExe(const std::shared_ptr<Variable>& variable, const std::shared_ptr<IDataProvider>& provider,
                   const std::vector<DateTimeRange>& ranges, DateTimeRange range, DateTimeRange cacheRange)
        :_variable{variable}, _provider{provider},_ranges{ranges},_range{range},_cacheRange{cacheRange}
    {
        setAutoDelete(true);
    }
    void run()override
    {
        std::vector<IDataSeries*> data;
        for(auto range:_ranges)
        {
            data.push_back(_provider->getData(DataProviderParameters{{range}, _variable->metadata()}));
        }
        _variable->updateData(data, _range, _cacheRange, true);
        emit transactionComplete();
    }
signals:
    void transactionComplete();
};
