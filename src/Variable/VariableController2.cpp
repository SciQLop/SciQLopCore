#include "Variable/VariableController2.h"
#include "Variable/VariableSynchronizationGroup2.h"
#include <Common/containers.h>
#include <Common/debug.h>
#include <Data/DataProviderParameters.h>

class VariableController2::VariableController2Private
{
    std::set<std::shared_ptr<Variable>> _variables;
    QMap<QUuid,std::shared_ptr<IDataProvider>> _providers;
    QMap<QUuid,std::shared_ptr<VariableSynchronizationGroup2>> _synchronizationGroups;
public:
    VariableController2Private(QObject* parent=Q_NULLPTR)
    {
        Q_UNUSED(parent);
    }

    ~VariableController2Private() = default;

    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider, const DateTimeRange &range)
    {
        auto newVar = std::make_shared<Variable>(name,metadata);
        this->_variables.insert(newVar);
        this->_providers[newVar->ID()] = provider;
        this->_synchronizationGroups[newVar->ID()] = std::make_shared<VariableSynchronizationGroup2>(newVar->ID());
        return newVar;
    }

    void deleteVariable(std::shared_ptr<Variable> variable)
    {
        if(this->_providers.contains(variable->ID()))
            this->_providers.remove(variable->ID());
        if(SciQLop::containers::contains(this->_variables, variable))
            this->_variables.erase(variable);
    }

    void changeRange(std::shared_ptr<Variable> variable, DateTimeRange r)
    {
        if(_providers.contains(variable->ID()))
        {
            auto provider = _providers[variable->ID()];
            auto data = provider->getData(DataProviderParameters{{r},variable->metadata()});
            variable->mergeDataSeries(data);
        }
        else
        {
            SCIQLOP_ERROR("No provider found for given variable");
        }
    }


    const std::set<std::shared_ptr<Variable>>& variables()
    {
        return  _variables;
    }

};

VariableController2::VariableController2()
    :impl{spimpl::make_unique_impl<VariableController2Private>()}
{}

std::shared_ptr<Variable> VariableController2::createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider, const DateTimeRange &range)
{
    auto var =  impl->createVariable(name, metadata, provider, range);
    emit variableAdded(var);
    impl->changeRange(var,range);
    return var;
}

void VariableController2::deleteVariable(std::shared_ptr<Variable> variable)
{
    impl->deleteVariable(variable);
    emit variableDeleted(variable);
}

void VariableController2::changeRange(std::shared_ptr<Variable> variable, DateTimeRange r)
{
    impl->changeRange(variable, r);
}

const std::set<std::shared_ptr<Variable> > &VariableController2::variables()
{
    return impl->variables();
}

