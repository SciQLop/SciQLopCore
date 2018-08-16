#include <QQueue>
#include <QThreadPool>
#include <QRunnable>
#include <QObject>

#include "Variable/VariableController2.h"
#include "Variable/VariableSynchronizationGroup2.h"
#include <Common/containers.h>
#include <Common/debug.h>
#include <Data/DataProviderParameters.h>
#include <Data/DateTimeRangeHelper.h>
#include <Data/DateTimeRange.h>
#include <Variable/VariableCacheStrategyFactory.h>
#include <Variable/private/VCTransaction.h>
#include <QCoreApplication>

class Transactions
{
    QReadWriteLock _mutex{QReadWriteLock::Recursive};
    std::map<QUuid,std::optional<std::shared_ptr<VCTransaction>>> _nextTransactions;
    std::map<QUuid,std::optional<std::shared_ptr<VCTransaction>>> _pendingTransactions;
public:
    void addEntry(QUuid id)
    {
        QWriteLocker lock{&_mutex};
        _nextTransactions[id] = std::nullopt;
        _pendingTransactions[id] = std::nullopt;
    }

    void removeEntry(QUuid id)
    {
        QWriteLocker lock{&_mutex};
        _nextTransactions.erase(id);
        _pendingTransactions.erase(id);
    }

    std::map<QUuid,std::optional<std::shared_ptr<VCTransaction>>> pendingTransactions()
    {
        QReadLocker lock{&_mutex};
        return _pendingTransactions;
    }

    std::map<QUuid,std::optional<std::shared_ptr<VCTransaction>>> nextTransactions()
    {
        QReadLocker lock{&_mutex};
        return _nextTransactions;
    }

    std::optional<std::shared_ptr<VCTransaction>> start(QUuid id)
    {
        QWriteLocker lock{&_mutex};
        _pendingTransactions[id] = _nextTransactions[id];
        _nextTransactions[id] = std::nullopt;
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
        return _nextTransactions[id].has_value() || _pendingTransactions[id].has_value();
    }
};

class VariableController2::VariableController2Private
{
    QThreadPool _ThreadPool;
    QMap<QUuid,std::shared_ptr<Variable>> _variables;
    QMap<QUuid,std::shared_ptr<IDataProvider>> _providers;
    QMap<QUuid,std::shared_ptr<VariableSynchronizationGroup2>> _synchronizationGroups;
    Transactions _transactions;
    QReadWriteLock _lock{QReadWriteLock::Recursive};
    std::unique_ptr<VariableCacheStrategy> _cacheStrategy;

    bool p_contains(const std::shared_ptr<Variable>& variable)
    {
        QReadLocker lock{&_lock};
        return _providers.contains(*variable);
    }
    bool v_contains(const std::shared_ptr<Variable>& variable)
    {
        QReadLocker lock{&_lock};
        return SciQLop::containers::contains(this->_variables, variable);
    }
    bool sg_contains(const std::shared_ptr<Variable>& variable)
    {
        QReadLocker lock{&_lock};
        return _synchronizationGroups.contains(*variable);
    }

    void _transactionComplete(QUuid group, std::shared_ptr<VCTransaction> transaction)
    {
        {
            QWriteLocker lock{&transaction->lock};
            transaction->ready -=1;
        }
        if(transaction->ready==0)
        {
            _transactions.complete(group);
        }
        this->_processTransactions();
    }
    void _processTransactions()
    {
        QWriteLocker lock{&_lock};
        auto nextTransactions = _transactions.nextTransactions();
        auto pendingTransactions = _transactions.pendingTransactions();
        for( auto [groupID, newTransaction] : nextTransactions)
        {
            if(newTransaction.has_value() && !pendingTransactions[groupID].has_value())
            {
                _transactions.start(groupID);
                auto refVar = _variables[newTransaction.value()->refVar];
                auto ranges = _computeAllRangesInGroup(refVar,newTransaction.value()->range);
                for( auto const& [ID, range] : ranges)
                {
                    auto provider = _providers[ID];
                    auto variable = _variables[ID];
                    auto [missingRanges, newCacheRange] = _computeMissingRanges(variable,range);
                    auto exe = new TransactionExe(_variables[ID], provider, missingRanges, range, newCacheRange);
                    QObject::connect(exe,
                            &TransactionExe::transactionComplete,
                            [groupID=groupID,transaction=newTransaction.value(),this]()
                            {
                                this->_transactionComplete(groupID, transaction);
                            }
                    );
                    _ThreadPool.start(exe);
                }
            }
        }
    }

    std::map<QUuid,DateTimeRange> _computeAllRangesInGroup(const std::shared_ptr<Variable>& refVar, DateTimeRange r)
    {
        std::map<QUuid,DateTimeRange> ranges;
        if(!DateTimeRangeHelper::hasnan(r))
        {
            auto group = _synchronizationGroups[*refVar];
            if(auto transformation = DateTimeRangeHelper::computeTransformation(refVar->range(),r);
                    transformation.has_value())
            {
                for(auto varId:group->variables())
                {
                    auto var = _variables[varId];
                    auto newRange = var->range().transform(transformation.value());
                    ranges[varId] = newRange;
                }
            }
            else // force new range to all variables -> may be weird if more than one var in the group
                // @TODO ensure that there is no side effects
            {
                for(auto varId:group->variables())
                {
                    auto var = _variables[varId];
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

    std::pair<std::vector<DateTimeRange>,DateTimeRange> _computeMissingRanges(const std::shared_ptr<Variable>& var, DateTimeRange r)
    {
        DateTimeRange newCacheRange;
        std::vector<DateTimeRange> missingRanges;
        if(DateTimeRangeHelper::hasnan(var->cacheRange()))
        {
            newCacheRange = _cacheStrategy->computeRange(r,r);
            missingRanges = {newCacheRange};
        }
        else
        {
            newCacheRange = _cacheStrategy->computeRange(var->cacheRange(),r);
            missingRanges = newCacheRange - var->cacheRange();
        }
        return {missingRanges,newCacheRange};
    }

    void _changeRange(QUuid id, DateTimeRange r)
    {
        _lock.lockForRead();
        auto var = _variables[id];
        _lock.unlock();
        _changeRange(var,r);
    }
    void _changeRange(const std::shared_ptr<Variable>& var, DateTimeRange r)
    {
        auto provider = _providers[*var];
        auto [missingRanges, newCacheRange] = _computeMissingRanges(var,r);
        std::vector<IDataSeries*> data;
        for(auto range:missingRanges)
        {
            data.push_back(provider->getData(DataProviderParameters{{range}, var->metadata()}));
        }
        var->updateData(data, r, newCacheRange, true);
    }
public:
    VariableController2Private(QObject* parent=Q_NULLPTR)
        :_cacheStrategy(VariableCacheStrategyFactory::createCacheStrategy(CacheStrategy::SingleThreshold))
    {
        Q_UNUSED(parent);
        this->_ThreadPool.setMaxThreadCount(32);
    }

    ~VariableController2Private()
    {
        while (this->_ThreadPool.activeThreadCount())
        {
            this->_ThreadPool.waitForDone(100);
        }
    }

    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider)
    {
        QWriteLocker lock{&_lock};
        auto newVar = std::make_shared<Variable>(name,metadata);
        this->_variables[*newVar] = newVar;
        this->_providers[*newVar] = std::move(provider);
        auto group = std::make_shared<VariableSynchronizationGroup2>(newVar->ID());
        this->_synchronizationGroups[*newVar] = group;
        this->_transactions.addEntry(*group);
        return newVar;
    }

    bool hasPendingTransactions(const std::shared_ptr<Variable>& variable)
    {
        QReadLocker lock{&_lock};
        auto group = _synchronizationGroups[*variable];
        return _transactions.active(*group);
    }

    void deleteVariable(const std::shared_ptr<Variable>& variable)
    {
        /*
         * Removing twice a var is ok but a var without provider has to be a hard error
         * this means we got the var controller in an inconsistent state
         */
        if(v_contains(variable))
        {
            QWriteLocker lock{&_lock};
            this->_variables.remove(*variable);
        }
        if(p_contains(variable))
        {
            QWriteLocker lock{&_lock};
            this->_providers.remove(*variable);
        }
        else
            SCIQLOP_ERROR(VariableController2Private, "No provider found for given variable");
    }

    void asyncChangeRange(const std::shared_ptr<Variable>& variable, const DateTimeRange& r)
    {
        if(p_contains(variable))
        {
            if(!DateTimeRangeHelper::hasnan(r))
            {
                auto group = _synchronizationGroups[*variable];
                // Just overwrite next transaction
                {
                    QWriteLocker lock{&_lock};
                    _transactions.enqueue(*group,std::make_shared<VCTransaction>(variable->ID(), r, static_cast<int>(group->variables().size())));
                }
                _processTransactions();
            }
            else
            {
                SCIQLOP_ERROR(VariableController2Private, "Invalid range containing NaN");
            }
        }
        else
        {
            SCIQLOP_ERROR(VariableController2Private, "No provider found for given variable");
        }
    }

    void changeRange(const std::shared_ptr<Variable>& variable, DateTimeRange r)
    {
        asyncChangeRange(variable,r);
        while (hasPendingTransactions(variable))
        {
            QCoreApplication::processEvents();
        }
    }

    void synchronize(const std::shared_ptr<Variable>& var, const std::shared_ptr<Variable>& with)
    {
        if(v_contains(var) && v_contains(with))
        {
            if(sg_contains(var) && sg_contains(with))
            {
                QWriteLocker lock{&_lock};
                auto dest_group = this->_synchronizationGroups[with->ID()];
                this->_synchronizationGroups[*var] = dest_group;
                dest_group->addVariable(*var);
            }
            else
            {
                SCIQLOP_ERROR(VariableController2Private, "At least one of the given variables isn't in a sync group");
            }
        }
        else
        {
            SCIQLOP_ERROR(VariableController2Private, "At least one of the given variables is not found");
        }
    }

    const std::set<std::shared_ptr<Variable>> variables()
    {
        std::set<std::shared_ptr<Variable>> vars;
        QReadLocker lock{&_lock};
        for(const auto  &var:_variables)
        {
            vars.insert(var);
        }
        return  vars;
    }

};

VariableController2::VariableController2()
    :impl{spimpl::make_unique_impl<VariableController2Private>()}
{}

std::shared_ptr<Variable> VariableController2::createVariable(const QString &name, const QVariantHash &metadata, const std::shared_ptr<IDataProvider>& provider, const DateTimeRange &range)
{
    auto var =  impl->createVariable(name, metadata, provider);
    emit variableAdded(var);
    if(!DateTimeRangeHelper::hasnan(range))
        impl->changeRange(var,range);
    else
        SCIQLOP_ERROR(VariableController2, "Creating a variable with default constructed DateTimeRange is an error");
    return var;
}

void VariableController2::deleteVariable(const std::shared_ptr<Variable>& variable)
{
    impl->deleteVariable(variable);
    emit variableDeleted(variable);
}

void VariableController2::changeRange(const std::shared_ptr<Variable>& variable, const DateTimeRange& r)
{
    impl->changeRange(variable, r);
}

void VariableController2::asyncChangeRange(const std::shared_ptr<Variable> &variable, const DateTimeRange &r)
{
    impl->asyncChangeRange(variable, r);
}

const std::set<std::shared_ptr<Variable> > VariableController2::variables()
{
    return impl->variables();
}

bool VariableController2::isReady(const std::shared_ptr<Variable> &variable)
{
    return impl->hasPendingTransactions(variable);
}

void VariableController2::synchronize(const std::shared_ptr<Variable> &var, const std::shared_ptr<Variable> &with)
{
    impl->synchronize(var, with);
}
