/*------------------------------------------------------------------------------
 * --  This file is a part of SciQLop
 * --  Copyright (C) 2019, Plasma Physics Laboratory - CNRS
 * --
 * --  This program is free software; you can redistribute it and/or modify
 * --  it under the terms of the GNU General Public License as published by
 * --  the Free Software Foundation; either version 3 of the License, or
 * --  (at your option) any later version.
 * --
 * --  This program is distributed in the hope that it will be useful,
 * --  but WITHOUT ANY WARRANTY; without even the implied warranty of
 * --  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * --  GNU General Public License for more details.
 * --
 * --  You should have received a copy of the GNU General Public License
 * --  along with this program; if not, write to the Free Software
 * --  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * -------------------------------------------------------------------------------*/
/* --                  Author : Alexis Jeandet
 * --                     Mail : alexis.jeandet@lpp.polytechnique.fr
 * --                            alexis.jeandet@member.fsf.org
 * ----------------------------------------------------------------------------*/
#pragma once

#include "DataSourceItem.hpp"
#include "SciQLopCore/Data/IDataProvider.hpp"

#include <QAbstractItemModel>
#include <QMimeData>
#include <QObject>
#include <QStringListModel>

class DataSources : public QAbstractItemModel
{
  Q_OBJECT

public:
  enum Roles
  {
    CustomRole = Qt::UserRole
  };

public:
  DataSources();
  ~DataSources();

  virtual QVariant data(const QModelIndex& index, int role) const final;
  virtual QMimeData* mimeData(const QModelIndexList& indexes) const final;

  virtual int columnCount(const QModelIndex& parent) const final;

  virtual int rowCount(const QModelIndex& parent) const final;

  QModelIndex parent(const QModelIndex& index) const final;

  virtual QModelIndex index(int row, int column,
                            const QModelIndex& parent) const final;

  Qt::ItemFlags flags(const QModelIndex& index) const final;

  void addDataSourceItem(const QUuid& providerUid, const QString& path,
                         const QMap<QString, QString>& metaData) noexcept;

  template<typename data_provider_t, class... Args>
  void addProvider(Args&&... args) noexcept
  {
    addProvider(new data_provider_t{std::forward<Args>(args)...});
  }

  void addProvider(IDataProvider* provider) noexcept;

  void updateNodeMetaData(const QString& path,
                          const QMap<QString, QString>& metaData) noexcept;

  void addIcon(const QString& name, QVariant icon)
  {
    _icons[name] = std::move(icon);
  }

  void setIcon(const QString& path, const QString& iconName);

  inline QStringListModel* completionModel() const noexcept
  {
    return _completionModel;
  }

  std::shared_ptr<IDataProvider> provider(const QString& path);

private:
  void _updateCompletionModel(const QMap<QString, QString>& metaData,
                              const QString& name);
  DataSourceItem* _root;
  std::map<QUuid, std::shared_ptr<IDataProvider>> _DataProviders;
  QHash<QString, QVariant> _icons;
  QStringListModel* _completionModel;
};
