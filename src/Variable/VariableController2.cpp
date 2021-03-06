#include "Variable/VariableController2.h"

#include "Variable/VariableSynchronizationGroup2.h"

#include <containers/algorithms.hpp>
#include <Common/debug.h>
#include <Data/DataProviderParameters.h>
#include <Data/DateTimeRange.h>
#include <Data/DateTimeRangeHelper.h>
#include <QCoreApplication>
#include <QDataStream>
#include <QObject>
#include <QQueue>
#include <QRunnable>
#include <QThreadPool>
#include <Variable/private/VCTransaction.h>

class VariableController2::VariableController2Private
{
  struct threadSafeVaraiblesMaps
  {
    inline void
    addVariable(const std::shared_ptr<Variable2>& variable,
                const std::shared_ptr<IDataProvider>& provider,
                const std::shared_ptr<VariableSynchronizationGroup2>&
                    synchronizationGroup)
    {
      QWriteLocker lock{&_lock};
      _variables[*variable]             = variable;
      _providers[*variable]             = provider;
      _synchronizationGroups[*variable] = synchronizationGroup;
    }

    inline void removeVariable(const std::shared_ptr<Variable2>& variable)
    {
      QWriteLocker lock{&_lock};
      _variables.erase(*variable);
      _providers.remove(*variable);
      _synchronizationGroups.remove(*variable);
    }

    inline void
    synchronize(const std::shared_ptr<Variable2>& variable,
                const std::optional<std::shared_ptr<Variable2>>& with)
    {
      QWriteLocker lock{&_lock};
      if(with.has_value())
      {
        auto newGroup = _synchronizationGroups[*with.value()];
        newGroup->addVariable(*variable);
        _synchronizationGroups[*variable] = newGroup;
      }
      else
      {
        _synchronizationGroups[*variable] =
            std::make_shared<VariableSynchronizationGroup2>(*variable);
      }
    }

    inline std::shared_ptr<Variable2> variable(QUuid variable)
    {
      QReadLocker lock{&_lock};
      auto it = _variables.find(variable);
#if __cplusplus > 201703L
      [[unlikely]]
#endif
      if(it == _variables.end())
          SCIQLOP_ERROR(threadSafeVaraiblesMaps, "Unknown Variable");
      return (*it).second;
    }

    inline std::shared_ptr<Variable2> variable(int index)
    {
      QReadLocker lock{&_lock};
#if __cplusplus > 201703L
      [[unlikely]]
#endif
      if(!(_variables.size() > static_cast<unsigned int>(index)))
          SCIQLOP_ERROR(threadSafeVaraiblesMaps, "Index is out of bounds");
      auto it = _variables.cbegin();
      while(index != 0)
      {
        index -= 1;
        it++;
      }
      return (*(it)).second;
    }

    inline const std::vector<std::shared_ptr<Variable2>> variables()
    {
      std::vector<std::shared_ptr<Variable2>> vars;
      QReadLocker lock{&_lock};
      for(const auto& [id, var] : _variables)
      {
        vars.push_back(var);
      }
      return vars;
    }

    inline std::shared_ptr<IDataProvider> provider(QUuid variable)
    {
      QReadLocker lock{&_lock};
#if __cplusplus > 201703L
      [[unlikely]]
#endif
      if(!_providers.contains(variable))
          SCIQLOP_ERROR(threadSafeVaraiblesMaps, "Unknown Variable");
      return _providers[variable];
    }

    inline std::shared_ptr<VariableSynchronizationGroup2> group(QUuid variable)
    {
      QReadLocker lock{&_lock};
#if __cplusplus > 201703L
      [[unlikely]]
#endif
      if(!_synchronizationGroups.contains(variable))
          SCIQLOP_ERROR(threadSafeVaraiblesMaps, "Unknown Variable");
      return _synchronizationGroups[variable];
    }

    inline bool has(const std::shared_ptr<Variable2>& variable)
    {
      QReadLocker lock{&_lock};
      return _variables.find(*variable) == _variables.end();
    }

  private:
    std::map<QUuid, std::shared_ptr<Variable2>> _variables;
    QMap<QUuid, std::shared_ptr<IDataProvider>> _providers;
    QMap<QUuid, std::shared_ptr<VariableSynchronizationGroup2>>
        _synchronizationGroups;
    QReadWriteLock _lock{QReadWriteLock::Recursive};
  } _maps;
  std::vector<QUuid> _variablesToRemove;
  QThreadPool* _ThreadPool;
  VCTransactionsQueues _transactions;

  void _transactionComplete(QUuid group,
                            std::shared_ptr<VCTransaction> transaction)
  {
    if(transaction->done()) { _transactions.complete(group); }
    this->_processTransactions();
  }

  void _cleanupVariables()
  {
    for(auto id : _variablesToRemove)
    {
      auto v = this->variable(id);
      if(!hasPendingTransactions(v))
      {
        _variablesToRemove.erase(std::remove(_variablesToRemove.begin(),
                                             _variablesToRemove.end(), id),
                                 _variablesToRemove.end());
        this->deleteVariable(v);
      }
    }
  }

  void _processTransactions(bool fragmented = false)
  {
    auto nextTransactions    = _transactions.nextTransactions();
    auto pendingTransactions = _transactions.pendingTransactions();
    for(auto [groupID, newTransaction] : nextTransactions)
    {
      if(newTransaction.has_value() &&
         !pendingTransactions[groupID].has_value())
      {
        _transactions.start(groupID);
        auto refVar = _maps.variable(newTransaction.value()->refVar);
        auto ranges =
            _computeAllRangesInGroup(refVar, newTransaction.value()->range);
        for(auto const& [ID, range] : ranges)
        {
          auto provider = _maps.provider(ID);
          auto variable = _maps.variable(ID);
          if(fragmented)
          {
            auto missingRanges = _computeMissingRanges(variable, range);

            auto exe =
                new TransactionExe(variable, provider, missingRanges, range);
            QObject::connect(
                exe, &TransactionExe::transactionComplete,
                [groupID = groupID, transaction = newTransaction.value(),
                 this]() { this->_transactionComplete(groupID, transaction); });
            _ThreadPool->start(exe);
          }
          else
          {
            auto exe = new TransactionExe(variable, provider, {range}, range);
            QObject::connect(
                exe, &TransactionExe::transactionComplete,
                [groupID = groupID, transaction = newTransaction.value(),
                 this]() { this->_transactionComplete(groupID, transaction); });
            _ThreadPool->start(exe);
          }
        }
      }
    }
    // after each transaction update we get a new distribution of idle and
    // working variables so we can delete variables which are waiting to be
    // deleted if they are now idle
    _cleanupVariables();
  }

  std::map<QUuid, DateTimeRange>
  _computeAllRangesInGroup(const std::shared_ptr<Variable2>& refVar,
                           DateTimeRange r)
  {
    std::map<QUuid, DateTimeRange> ranges;
    if(!DateTimeRangeHelper::hasnan(r))
    {
      auto group = _maps.group(*refVar);
      if(auto transformation =
             DateTimeRangeHelper::computeTransformation(refVar->range(), r);
         transformation.has_value())
      {
        for(auto varId : group->variables())
        {
          auto var      = _maps.variable(varId);
          auto newRange = var->range().transform(transformation.value());
          ranges[varId] = newRange;
        }
      }
      else // force new range to all variables -> may be weird if more than one
           // var in the group
           // TODO ensure that there is no side effects
      {
        for(auto varId : group->variables())
        {
          auto var      = _maps.variable(varId);
          ranges[varId] = r;
        }
      }
    }
    else
    {
      SCIQLOP_ERROR(VariableController2Private, "Invalid range containing NaN");
    }
    return ranges;
  }

  std::vector<DateTimeRange>
  _computeMissingRanges(const std::shared_ptr<Variable2>& var, DateTimeRange r)
  {
    return r - var->range();
  }

  void _changeRange(QUuid id, DateTimeRange r)
  {
    _changeRange(_maps.variable(id), r);
  }
  void _changeRange(const std::shared_ptr<Variable2>& var, DateTimeRange r)
  {
    auto provider      = _maps.provider(*var);
    auto missingRanges = _computeMissingRanges(var, r);
    std::vector<TimeSeries::ITimeSerie*> data;
    for(auto range : missingRanges)
    {
      data.push_back(
          provider->getData(DataProviderParameters{{range}, var->metadata()}));
    }
    data.push_back(var->data().get()); // might be smarter
    var->setData(data, r, true);
    std::for_each(std::begin(data), std::end(data), [](auto ts) { delete ts; });
  }

public:
  VariableController2Private(QObject* parent = Q_NULLPTR)
  {
    Q_UNUSED(parent);
    this->_ThreadPool = new QThreadPool();
    this->_ThreadPool->setMaxThreadCount(32);
  }

  /*
   * This dtor has to like this even if this is ugly, because default dtor would
   * rely on declaration order to destruct members and that would always lead to
   * regressions when modifying class members
   */
  ~VariableController2Private() { delete this->_ThreadPool; }

  std::shared_ptr<Variable2>
  createVariable(const QString& name, const QVariantHash& metadata,
                 std::shared_ptr<IDataProvider> provider)
  {
    auto newVar = std::make_shared<Variable2>(name, metadata);
    auto group  = std::make_shared<VariableSynchronizationGroup2>(newVar->ID());
    _maps.addVariable(newVar, std::move(provider), group);
    this->_transactions.addEntry(*group);
    return newVar;
  }

  std::shared_ptr<Variable2> variable(QUuid ID) { return _maps.variable(ID); }

  std::shared_ptr<Variable2> variable(int index)
  {
    return _maps.variable(index);
  }

  std::shared_ptr<Variable2>
  cloneVariable(const std::shared_ptr<Variable2>& variable)
  {
    auto newVar = variable->clone();
    _maps.synchronize(newVar, std::nullopt);
    _maps.addVariable(newVar, _maps.provider(*variable), _maps.group(*newVar));
    this->_transactions.addEntry(*_maps.group(*newVar));
    return newVar;
  }

  bool hasPendingTransactions(const std::shared_ptr<Variable2>& variable)
  {
    return _transactions.active(*_maps.group(*variable));
  }

  bool hasPendingTransactions()
  {
    bool has = false;
    for(const auto& var : _maps.variables())
    {
      has |= _transactions.active(*_maps.group(*var));
    }
    return has;
  }

  void deleteVariable(const std::shared_ptr<Variable2>& variable)
  {
    if(!hasPendingTransactions(variable))
      _maps.removeVariable(variable);
    else
      _variablesToRemove.push_back(variable->ID());
  }

  void asyncChangeRange(const std::shared_ptr<Variable2>& variable,
                        const DateTimeRange& r)
  {
    if(!DateTimeRangeHelper::hasnan(r))
    {
      auto group = _maps.group(*variable);
      // Just overwrite next transaction
      {
        _transactions.enqueue(*group,
                              std::make_shared<VCTransaction>(
                                  variable->ID(), r,
                                  static_cast<int>(group->variables().size())));
      }
      _processTransactions();
    }
    else
    {
      SCIQLOP_ERROR(VariableController2Private, "Invalid range containing NaN");
    }
  }

  void changeRange(const std::shared_ptr<Variable2>& variable, DateTimeRange r)
  {
    asyncChangeRange(variable, r);
    while(hasPendingTransactions(variable))
    {
      QCoreApplication::processEvents();
    }
  }

  inline void synchronize(const std::shared_ptr<Variable2>& var,
                          const std::shared_ptr<Variable2>& with)
  {
    _maps.synchronize(var, with);
  }

  inline const std::vector<std::shared_ptr<Variable2>> variables()
  {
    return _maps.variables();
  }
};

VariableController2::VariableController2()
    : impl{spimpl::make_unique_impl<VariableController2Private>()}
{}

std::shared_ptr<Variable2> VariableController2::createVariable(
    const QString& name, const QVariantHash& metadata,
    const std::shared_ptr<IDataProvider>& provider, const DateTimeRange& range)
{
  auto var = impl->createVariable(name, metadata, provider);
  var->setRange(range); // even with no data this is it's range
  if(!DateTimeRangeHelper::hasnan(range))
    impl->asyncChangeRange(var, range);
  else
    SCIQLOP_ERROR(VariableController2, "Creating a variable with default "
                                       "constructed DateTimeRange is an error");
  emit variableAdded(var);
  return var;
}

std::shared_ptr<Variable2>
VariableController2::cloneVariable(const std::shared_ptr<Variable2>& variable)
{
  return impl->cloneVariable(variable);
}

void VariableController2::deleteVariable(
    const std::shared_ptr<Variable2>& variable)
{
  impl->deleteVariable(variable);
  emit variableDeleted(variable);
}

void VariableController2::changeRange(
    const std::shared_ptr<Variable2>& variable, const DateTimeRange& r)
{
  impl->changeRange(variable, r);
}

void VariableController2::asyncChangeRange(
    const std::shared_ptr<Variable2>& variable, const DateTimeRange& r)
{
  impl->asyncChangeRange(variable, r);
}

const std::vector<std::shared_ptr<Variable2>> VariableController2::variables()
{
  return impl->variables();
}

bool VariableController2::isReady(const std::shared_ptr<Variable2>& variable)
{
  return !impl->hasPendingTransactions(variable);
}

bool VariableController2::isReady() { return !impl->hasPendingTransactions(); }

void VariableController2::synchronize(const std::shared_ptr<Variable2>& var,
                                      const std::shared_ptr<Variable2>& with)
{
  impl->synchronize(var, with);
}

const std::vector<std::shared_ptr<Variable2>>
VariableController2::variables(const std::vector<QUuid>& ids)
{
  std::vector<std::shared_ptr<Variable2>> variables;
  std::transform(std::cbegin(ids), std::cend(ids),
                 std::back_inserter(variables),
                 [this](const auto& id) { return impl->variable(id); });
  return variables;
}
