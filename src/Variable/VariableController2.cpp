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
    struct threadSafeVaraiblesMaps
    {
        inline void addVariable(const std::shared_ptr<Variable>& variable, const std::shared_ptr<IDataProvider>& provider, const std::shared_ptr<VariableSynchronizationGroup2>& synchronizationGroup)
        {
            QWriteLocker lock{&_lock};
            _variables[*variable] = variable;
            _providers[*variable] = provider;
            _synchronizationGroups[*variable] = synchronizationGroup;
        }

        inline void removeVariable(const std::shared_ptr<Variable>& variable)
        {
            QWriteLocker lock{&_lock};
            _variables.remove(*variable);
            _providers.remove(*variable);
            _synchronizationGroups.remove(*variable);
        }

        inline void synchronize(const std::shared_ptr<Variable>& variable, const std::optional<std::shared_ptr<Variable>>& with)
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
                _synchronizationGroups[*variable] = std::make_shared<VariableSynchronizationGroup2>(*variable);
            }
        }

        inline std::shared_ptr<Variable> variable(QUuid variable)
        {
            QReadLocker lock{&_lock};
            [[unlikely]]
            if(!_variables.contains(variable))
                SCIQLOP_ERROR(threadSafeVaraiblesMaps,"Unknown Variable");
            return _variables[variable];
        }

        inline const std::set<std::shared_ptr<Variable>> variables()
        {
            std::set<std::shared_ptr<Variable>> vars;
            QReadLocker lock{&_lock};
            for(const auto  &var:_variables)
            {
                vars.insert(var);
            }
            return  vars;
        }

        inline std::shared_ptr<IDataProvider> provider(QUuid variable)
        {
            QReadLocker lock{&_lock};
            [[unlikely]]
            if(!_providers.contains(variable))
                SCIQLOP_ERROR(threadSafeVaraiblesMaps,"Unknown Variable");
            return _providers[variable];
        }

        inline std::shared_ptr<VariableSynchronizationGroup2> group(QUuid variable)
        {
            QReadLocker lock{&_lock};
            [[unlikely]]
            if(!_synchronizationGroups.contains(variable))
                SCIQLOP_ERROR(threadSafeVaraiblesMaps,"Unknown Variable");
            return _synchronizationGroups[variable];
        }

        inline bool has(const std::shared_ptr<Variable>& variable)
        {
            QReadLocker lock{&_lock};
            return _variables.contains(*variable);
        }

    private:
        QMap<QUuid,std::shared_ptr<Variable>> _variables;
        QMap<QUuid,std::shared_ptr<IDataProvider>> _providers;
        QMap<QUuid,std::shared_ptr<VariableSynchronizationGroup2>> _synchronizationGroups;
        QReadWriteLock _lock{QReadWriteLock::Recursive};
    }_maps;
    QThreadPool _ThreadPool;
    Transactions _transactions;
    std::unique_ptr<VariableCacheStrategy> _cacheStrategy;

    void _transactionComplete(QUuid group, std::shared_ptr<VCTransaction> transaction)
    {
        if(transaction->done())
        {
            _transactions.complete(group);
        }
        this->_processTransactions();
    }
    void _processTransactions()
    {
        auto nextTransactions = _transactions.nextTransactions();
        auto pendingTransactions = _transactions.pendingTransactions();
        for( auto [groupID, newTransaction] : nextTransactions)
        {
            if(newTransaction.has_value() && !pendingTransactions[groupID].has_value())
            {
                _transactions.start(groupID);
                auto refVar = _maps.variable(newTransaction.value()->refVar);
                auto ranges = _computeAllRangesInGroup(refVar,newTransaction.value()->range);
                for( auto const& [ID, range] : ranges)
                {
                    auto provider = _maps.provider(ID);
                    auto variable = _maps.variable(ID);
                    auto [missingRanges, newCacheRange] = _computeMissingRanges(variable,range);
                    auto exe = new TransactionExe(variable, provider, missingRanges, range, newCacheRange);
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
            auto group = _maps.group(*refVar);
            if(auto transformation = DateTimeRangeHelper::computeTransformation(refVar->range(),r);
                    transformation.has_value())
            {
                for(auto varId:group->variables())
                {
                    auto var = _maps.variable(varId);
                    auto newRange = var->range().transform(transformation.value());
                    ranges[varId] = newRange;
                }
            }
            else // force new range to all variables -> may be weird if more than one var in the group
                // @TODO ensure that there is no side effects
            {
                for(auto varId:group->variables())
                {
                    auto var = _maps.variable(varId);
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
        _changeRange(_maps.variable(id) ,r);
    }
    void _changeRange(const std::shared_ptr<Variable>& var, DateTimeRange r)
    {
        auto provider = _maps.provider(*var);
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

    /*
     * This dtor has to like this even if this is ugly, because default dtor would rely on
     * declaration order to destruct members and that would always lead to regressions when
     * modifying with class members
    */
    ~VariableController2Private()
    {
        while (this->_ThreadPool.activeThreadCount())
        {
            this->_ThreadPool.waitForDone(100);
        }
    }

    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider)
    {
        auto newVar = std::make_shared<Variable>(name,metadata);
        auto group = std::make_shared<VariableSynchronizationGroup2>(newVar->ID());
        _maps.addVariable(newVar,std::move(provider),group);
        this->_transactions.addEntry(*group);
        return newVar;
    }

    bool hasPendingTransactions(const std::shared_ptr<Variable>& variable)
    {
        return _transactions.active(*_maps.group(*variable));
    }

    void deleteVariable(const std::shared_ptr<Variable>& variable)
    {
        /*
         * Removing twice a var is ok but a var without provider has to be a hard error
         * this means we got the var controller in an inconsistent state
         */
        _maps.removeVariable(variable);
    }

    void asyncChangeRange(const std::shared_ptr<Variable>& variable, const DateTimeRange& r)
    {
        if(!DateTimeRangeHelper::hasnan(r))
        {
            auto group = _maps.group(*variable);
            // Just overwrite next transaction
            {
                _transactions.enqueue(*group,std::make_shared<VCTransaction>(variable->ID(), r, static_cast<int>(group->variables().size())));
            }
            _processTransactions();
        }
        else
        {
            SCIQLOP_ERROR(VariableController2Private, "Invalid range containing NaN");
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

    inline void synchronize(const std::shared_ptr<Variable>& var, const std::shared_ptr<Variable>& with)
    {
        _maps.synchronize(var, with);
    }

    inline const std::set<std::shared_ptr<Variable>> variables()
    {
        return _maps.variables();
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
