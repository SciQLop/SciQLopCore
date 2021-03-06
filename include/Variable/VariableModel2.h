#ifndef SCIQLOP_VARIABLEMODEL2_H
#define SCIQLOP_VARIABLEMODEL2_H

#include "CoreGlobal.h"

#include <Common/MetaTypes.h>
#include <Common/spimpl.h>
#include <Data/DateTimeRange.h>
#include <QAbstractTableModel>
#include <QLoggingCategory>
#include <QUuid>

class IDataSeries;
class Variable2;
class VariableController2;

enum VariableRoles
{
  ProgressRole = Qt::UserRole
};

/**
 * @brief The VariableModel class aims to hold the variables that have been
 * created in SciQlop
 */
class SCIQLOP_CORE_EXPORT VariableModel2 : public QAbstractTableModel
{
  Q_OBJECT
  // read only mirror of VariableController2 content
  std::vector<std::shared_ptr<Variable2>> _variables;

public:
  explicit VariableModel2(QObject* parent = nullptr);

  // /////////////////////////// //
  // QAbstractTableModel methods //
  // /////////////////////////// //

  virtual int
  columnCount(const QModelIndex& parent = QModelIndex{}) const override;
  virtual int
  rowCount(const QModelIndex& parent = QModelIndex{}) const override;
  virtual QVariant data(const QModelIndex& index,
                        int role = Qt::DisplayRole) const override;
  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const override;
  virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

  // ///////////////// //
  // Drag&Drop methods //
  // ///////////////// //

  virtual Qt::DropActions supportedDropActions() const override;
  virtual Qt::DropActions supportedDragActions() const override;
  virtual QStringList mimeTypes() const override;
  virtual QMimeData* mimeData(const QModelIndexList& indexes) const override;
  virtual bool canDropMimeData(const QMimeData* data, Qt::DropAction action,
                               int row, int column,
                               const QModelIndex& parent) const override;
  virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                            int row, int column,
                            const QModelIndex& parent) override;
  const std::vector<std::shared_ptr<Variable2>>& variables() const
  {
    return _variables;
  }

signals:
  void createVariable(const QString& productPath);
  void asyncChangeRange(const std::shared_ptr<Variable2>& variable,
                        const DateTimeRange& r);
public slots:
  /// Slot called when data of a variable has been updated
  void variableUpdated(QUuid id) noexcept;
  void variableAdded(const std::shared_ptr<Variable2>&);
  void variableDeleted(const std::shared_ptr<Variable2>&);
};

#endif // SCIQLOP_VARIABLEMODEL2_H
