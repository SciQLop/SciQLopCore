#include "Variable/VariableSynchronizationGroup2.h"

#include <Common/containers.h>
#include <Common/debug.h>
#include <Data/DataProviderParameters.h>
#include <Data/DateTimeRange.h>
#include <Data/DateTimeRangeHelper.h>
#include <Data/IDataProvider.h>
#include <QObject>
#include <QReadWriteLock>
#include <QRunnable>
#include <QThreadPool>
#include <TimeSeries.h>
#include <Variable/Variable2.h>

struct VCTransaction
{
  VCTransaction(QUuid refVar, DateTimeRange range, int varCount)
      : refVar{refVar}, range{range}, _remainingVars{varCount}
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
    _remainingVars -= 1;
    return _remainingVars == 0;
  }

private:
  QReadWriteLock _lock;
  int _remainingVars;
};

class TransactionExe : public QObject, public QRunnable
{
  Q_OBJECT
  std::shared_ptr<Variable2> _variable;
  std::shared_ptr<IDataProvider> _provider;
  std::vector<DateTimeRange> _ranges;
  DateTimeRange _range;
  bool _overwrite;

public:
  TransactionExe(const std::shared_ptr<Variable2>& variable,
                 const std::shared_ptr<IDataProvider>& provider,
                 const std::vector<DateTimeRange>& ranges, DateTimeRange range,
                 bool overwrite = true)
      : _variable{variable}, _provider{provider}, _ranges{ranges},
        _range{range}, _overwrite{overwrite}
  {
    setAutoDelete(true);
  }
  void run() override
  {
    std::vector<TimeSeries::ITimeSerie*> data;
    for(auto range : _ranges)
    {
      auto ds = _provider->getData(
          DataProviderParameters{{range}, _variable->metadata()});
      if(ds and ds->size()) data.push_back(ds); // skip empty dataSeries
    }
    if(_overwrite)
      _variable->setData(data, _range, true);
    else
    {
      data.push_back(_variable->data().get());
      _variable->setData(data, _range, true);
    }
    std::for_each(std::begin(data), std::end(data),
                  [](TimeSeries::ITimeSerie* ts) { delete ts; });
    emit transactionComplete();
  }
signals:
  void transactionComplete();
};

class VCTransactionsQueues
{
  QReadWriteLock _mutex{QReadWriteLock::Recursive};
  std::map<QUuid, std::optional<std::shared_ptr<VCTransaction>>>
      _nextTransactions;
  std::map<QUuid, std::optional<std::shared_ptr<VCTransaction>>>
      _pendingTransactions;

public:
  void addEntry(QUuid id)
  {
    QWriteLocker lock{&_mutex};
    _nextTransactions[id]    = std::nullopt;
    _pendingTransactions[id] = std::nullopt;
  }

  void removeEntry(QUuid id)
  {
    QWriteLocker lock{&_mutex};
    _nextTransactions.erase(id);
    _pendingTransactions.erase(id);
  }

  std::map<QUuid, std::optional<std::shared_ptr<VCTransaction>>>
  pendingTransactions()
  {
    QReadLocker lock{&_mutex};
    return _pendingTransactions;
  }

  std::map<QUuid, std::optional<std::shared_ptr<VCTransaction>>>
  nextTransactions()
  {
    QReadLocker lock{&_mutex};
    return _nextTransactions;
  }

  std::optional<std::shared_ptr<VCTransaction>> start(QUuid id)
  {
    QWriteLocker lock{&_mutex};
    _pendingTransactions[id] = _nextTransactions[id];
    _nextTransactions[id]    = std::nullopt;
    return _pendingTransactions[id];
  }

  void enqueue(QUuid id, std::shared_ptr<VCTransaction> transaction)
  {
    QWriteLocker lock{&_mutex};
    _nextTransactions[id] = transaction;
  }

  void complete(QUuid id)
  {
    QWriteLocker lock{&_mutex};
    _pendingTransactions[id] = std::nullopt;
  }

  bool active(QUuid id)
  {
    QReadLocker lock{&_mutex};
    return _nextTransactions[id].has_value() ||
           _pendingTransactions[id].has_value();
  }
};
