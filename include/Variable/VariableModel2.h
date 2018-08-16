#ifndef SCIQLOP_VARIABLEMODEL2_H
#define SCIQLOP_VARIABLEMODEL2_H

#include "CoreGlobal.h"

#include <Data/DateTimeRange.h>

#include <QAbstractTableModel>
#include <QLoggingCategory>

#include <Common/MetaTypes.h>
#include <Common/spimpl.h>



class IDataSeries;
class Variable;
class VariableController2;

/**
 * @brief The VariableModel class aims to hold the variables that have been created in SciQlop
 */
class SCIQLOP_CORE_EXPORT VariableModel2 : public QAbstractTableModel {
    enum VariableRoles { ProgressRole = Qt::UserRole };
    Q_OBJECT
    std::shared_ptr<VariableController2> _variableController;
public:
    explicit VariableModel2(const std::shared_ptr<VariableController2>& variableController, QObject *parent = nullptr);

    // /////////////////////////// //
    // QAbstractTableModel methods //
    // /////////////////////////// //

    virtual int columnCount(const QModelIndex &parent = QModelIndex{}) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex{}) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    // ///////////////// //
    // Drag&Drop methods //
    // ///////////////// //

    virtual Qt::DropActions supportedDropActions() const override;
    virtual Qt::DropActions supportedDragActions() const override;
    virtual QStringList mimeTypes() const override;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
    virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                 const QModelIndex &parent) const override;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                              const QModelIndex &parent) override;


signals:

private slots:
    /// Slot called when data of a variable has been updated
    void variableUpdated() noexcept;
};

#endif // SCIQLOP_VARIABLEMODEL2_H
