#include "Variable/VariableController2.h"


class VariableController2Private
{

public:
    VariableController2Private(QObject* parent=Q_NULLPTR)
    {}
    ~VariableController2Private() = default;
    std::shared_ptr<Variable> createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider, const DateTimeRange &range)
    {
        return std::make_shared<Variable>(name,metadata);
    }
};

VariableController2::VariableController2()
    :impl{spimpl::make_unique_impl<VariableController2Private>()}
{}

std::shared_ptr<Variable> VariableController2::createVariable(const QString &name, const QVariantHash &metadata, std::shared_ptr<IDataProvider> provider, const DateTimeRange &range)
{
    return impl->createVariable(name, metadata, provider, range);
}

