
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

#ifndef DATASOURCES_H
#define DATASOURCES_H

#include "Data/IDataProvider.h"
#include "DataSourceItem.h"

#include <QAbstractItemModel>

class DataSources : public QAbstractItemModel
{
  Q_OBJECT

public:
  enum Roles
  {
    CustomRole = Qt::UserRole
  };

public:
  DataSources() : _root{} {}
  ~DataSources() { delete _root; }

  virtual QVariant data(const QModelIndex& index, int role) const override;

  int columnCount(const QModelIndex& parent) const override;

  virtual int rowCount(const QModelIndex& parent) const override;

  QModelIndex parent(const QModelIndex& index) const override;

  virtual QModelIndex index(int row, int column,
                            const QModelIndex& parent) const override;

  void addDataSourceItem(const QUuid& providerUid, const QString& path,
                         const QMap<QString, QString>& metaData) noexcept;

  void addProvider(IDataProvider* provider) noexcept;

private:
  DataSourceItem* _root;
  std::map<QUuid, std::shared_ptr<IDataProvider>> _DataProviders;
};

#endif // DATASOURCES_H
