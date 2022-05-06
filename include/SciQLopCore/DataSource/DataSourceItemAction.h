#pragma once

#include <QLoggingCategory>
#include <QObject>
#include <QString>

#include <functional>
#include <memory>

Q_DECLARE_LOGGING_CATEGORY(LOG_DataSourceItemAction)

class DataSourceItem;

/**
 * @brief The DataSourceItemAction class represents an action on a data source item.
 *
 * An action is a function that will be executed when the slot execute() is called.
 */
class DataSourceItemAction : public QObject {

    Q_OBJECT

public:
    /// Signature of the function associated to the action
    using ExecuteFunction = std::function<void(DataSourceItem &dataSourceItem)>;

    /**
     * Ctor
     * @param name the name of the action
     * @param fun the function that will be called when the action is executed
     * @sa execute()
     */
    explicit DataSourceItemAction(const QString &name, ExecuteFunction fun);
    ~DataSourceItemAction();

    std::unique_ptr<DataSourceItemAction> clone() const;

    QString name() const noexcept;

    /// Sets the data source item concerned by the action
    void setDataSourceItem(DataSourceItem *dataSourceItem) noexcept;

public slots:
    /// Executes the action
    void execute();

private:
    class DataSourceItemActionPrivate;
    std::unique_ptr<DataSourceItemActionPrivate> impl;
};
