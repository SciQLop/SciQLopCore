#include <Common/DateUtils.h>
#include <Common/MimeTypesDef.h>
#include <Common/StringUtils.h>
#include <Common/containers.h>
#include <DataSource/DataSourceController.h>
#include <QMimeData>
#include <QSize>
#include <QTimer>
#include <Time/TimeController.h>
#include <Variable/Variable2.h>
#include <Variable/VariableController2.h>
#include <Variable/VariableModel2.h>
#include <unordered_map>

namespace
{
  // Column indexes
  const auto NAME_COLUMN     = 0;
  const auto TSTART_COLUMN   = 1;
  const auto TEND_COLUMN     = 2;
  const auto NBPOINTS_COLUMN = 3;
  const auto UNIT_COLUMN     = 4;
  const auto MISSION_COLUMN  = 5;
  const auto PLUGIN_COLUMN   = 6;
  const auto NB_COLUMNS      = 7;

  // Column properties
  const auto DEFAULT_HEIGHT = 25;
  const auto DEFAULT_WIDTH  = 100;

  struct ColumnProperties
  {
    ColumnProperties(const QString& name = {}, int width = DEFAULT_WIDTH,
                     int height = DEFAULT_HEIGHT)
        : m_Name{name}, m_Width{width}, m_Height{height}
    {}

    QString m_Name;
    int m_Width;
    int m_Height;
  };

  const auto COLUMN_PROPERTIES = QHash<int, ColumnProperties>{
      {NAME_COLUMN, {QObject::tr("Name")}},
      {TSTART_COLUMN, {QObject::tr("tStart"), 180}},
      {TEND_COLUMN, {QObject::tr("tEnd"), 180}},
      {NBPOINTS_COLUMN, {QObject::tr("Nb points")}},
      {UNIT_COLUMN, {QObject::tr("Unit")}},
      {MISSION_COLUMN, {QObject::tr("Mission")}},
      {PLUGIN_COLUMN, {QObject::tr("Plugin")}}};

  QString uniqueName(const QString& defaultName,
                     const std::vector<std::shared_ptr<Variable2>>& variables)
  {
    auto forbiddenNames = std::vector<QString>(variables.size());
    std::transform(variables.cbegin(), variables.cend(), forbiddenNames.begin(),
                   [](const auto& variable) { return variable->name(); });
    auto uniqueName = StringUtils::uniqueName(defaultName, forbiddenNames);
    Q_ASSERT(!uniqueName.isEmpty());

    return uniqueName;
  }

} // namespace

VariableModel2::VariableModel2(QObject* parent) : QAbstractTableModel{parent} {}

int VariableModel2::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);

  return NB_COLUMNS;
}

int VariableModel2::rowCount(const QModelIndex& parent) const
{
  Q_UNUSED(parent);
  return _variables.size();
}

QVariant VariableModel2::data(const QModelIndex& index, int role) const
{
  if(!index.isValid()) { return QVariant{}; }

  if(index.row() < 0 || index.row() >= rowCount()) { return QVariant{}; }

  if(role == Qt::DisplayRole)
  {
    if(auto variable = _variables[index.row()])
    {
      switch(index.column())
      {
        case NAME_COLUMN: return variable->name();
        case TSTART_COLUMN:
        {
          if(auto range = variable->realRange(); range.has_value())
            return DateUtils::dateTime(range.value().m_TStart)
                .toString(DATETIME_FORMAT);
          return QVariant{};
        }
        case TEND_COLUMN:
        {
          if(auto range = variable->realRange(); range.has_value())
            return DateUtils::dateTime(range.value().m_TEnd)
                .toString(DATETIME_FORMAT);
          return QVariant{};
        }
        case NBPOINTS_COLUMN: return int(variable->nbPoints());
        case UNIT_COLUMN:
          return variable->metadata().value(QStringLiteral("units"));
        case MISSION_COLUMN:
          return variable->metadata().value(QStringLiteral("mission"));
        case PLUGIN_COLUMN:
          return variable->metadata().value(QStringLiteral("plugin"));
        default:
          // No action
          break;
      }
    }
  }
  else if(role == VariableRoles::ProgressRole)
  {
    return QVariant{};
  }

  return QVariant{};
}

QVariant VariableModel2::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
  if(role != Qt::DisplayRole && role != Qt::SizeHintRole) { return QVariant{}; }

  if(orientation == Qt::Horizontal)
  {
    auto propertiesIt = COLUMN_PROPERTIES.find(section);
    if(propertiesIt != COLUMN_PROPERTIES.cend())
    {
      // Role is either DisplayRole or SizeHintRole
      return (role == Qt::DisplayRole)
                 ? QVariant{propertiesIt->m_Name}
                 : QVariant{
                       QSize{propertiesIt->m_Width, propertiesIt->m_Height}};
    }
  }

  return QVariant{};
}

Qt::ItemFlags VariableModel2::flags(const QModelIndex& index) const
{
  return QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled |
         Qt::ItemIsDropEnabled;
}

Qt::DropActions VariableModel2::supportedDropActions() const
{
  return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions VariableModel2::supportedDragActions() const
{
  return Qt::CopyAction | Qt::MoveAction;
}

QStringList VariableModel2::mimeTypes() const
{
  return {MIME_TYPE_VARIABLE_LIST, MIME_TYPE_TIME_RANGE};
}

QMimeData* VariableModel2::mimeData(const QModelIndexList& indexes) const
{
  auto mimeData = new QMimeData;
  std::vector<std::shared_ptr<Variable2>> variables;

  DateTimeRange firstTimeRange;
  for(const auto& index : indexes)
  {
    if(index.column() == 0)
    { // only the first column
      auto variable = _variables[index.row()];
      if(variable.get() && index.isValid())
      {
        if(variables.size() == 0)
        {
          // Gets the range of the first variable
          firstTimeRange = variable->range();
        }
        variables.push_back(variable);
      }
    }
  }

  auto variablesEncodedData = Variable2::mimeData(variables);
  mimeData->setData(MIME_TYPE_VARIABLE_LIST, variablesEncodedData);

  if(variables.size() == 1)
  {
    // No time range MIME data if multiple variables are dragged
    auto timeEncodedData = TimeController::mimeDataForTimeRange(firstTimeRange);
    mimeData->setData(MIME_TYPE_TIME_RANGE, timeEncodedData);
  }

  return mimeData;
}

bool VariableModel2::canDropMimeData(const QMimeData* data,
                                     Qt::DropAction action, int row, int column,
                                     const QModelIndex& parent) const
{
  Q_UNUSED(column);
  // drop of a product
  return data->hasFormat(MIME_TYPE_PRODUCT_LIST) ||
         (data->hasFormat(MIME_TYPE_TIME_RANGE) && parent.isValid() &&
          !data->hasFormat(MIME_TYPE_VARIABLE_LIST));
}

bool VariableModel2::dropMimeData(const QMimeData* data, Qt::DropAction action,
                                  int row, int column,
                                  const QModelIndex& parent)
{
  auto dropDone = false;

  if(data->hasFormat(MIME_TYPE_PRODUCT_LIST))
  {
    auto productList = DataSourceController::productsDataForMimeData(
        data->data(MIME_TYPE_PRODUCT_LIST));

    for(auto metaData : productList)
    {
      emit createVariable(metaData.toHash());
    }

    dropDone = true;
  }
  else if(data->hasFormat(MIME_TYPE_TIME_RANGE) && parent.isValid())
  {
    auto variable = _variables[parent.row()];
    auto range =
        TimeController::timeRangeForMimeData(data->data(MIME_TYPE_TIME_RANGE));

    emit asyncChangeRange(variable, range);

    dropDone = true;
  }

  return dropDone;
}

void VariableModel2::variableUpdated(QUuid id) noexcept
{
  emit dataChanged(QModelIndex(), QModelIndex());
}

void VariableModel2::variableAdded(const std::shared_ptr<Variable2>& variable)
{
  if(!SciQLop::containers::contains(_variables, variable))
  {
    beginInsertRows(QModelIndex(), this->_variables.size(),
                    this->_variables.size());
    this->_variables.push_back(variable);
    endInsertRows();
    connect(variable.get(), &Variable2::updated, this,
            &VariableModel2::variableUpdated);
  }
}

void VariableModel2::variableDeleted(const std::shared_ptr<Variable2>& variable)
{
  auto it = std::find(_variables.begin(), _variables.end(), variable);
  if(it != _variables.end())
  {
    auto index = std::distance(_variables.begin(), it);
    beginRemoveRows(QModelIndex(), index, index);
    _variables.erase(it);
    endRemoveRows();
  }
}
