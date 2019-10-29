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
#ifndef DATASOURCESFILTER_H_INCLUDED
#define DATASOURCESFILTER_H_INCLUDED

#include "DataSourceItem.h"

#include <QObject>
#include <QSortFilterProxyModel>

class DataSourceFilter : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  explicit DataSourceFilter(QObject* parent = nullptr);

  bool filterAcceptsRow(int sourceRow,
                        const QModelIndex& sourceParent) const override
  {
    if(filterRegExp().isEmpty()) { return true; }
    DataSourceItem* childItem = static_cast<DataSourceItem*>(
        sourceModel()->index(sourceRow, 0, sourceParent).internalPointer());
    if(childItem)
    {
      auto data = childItem->data();
      return (std::find_if(std::cbegin(data), std::cend(data),
                           [regex = filterRegExp()](const auto& item) {
                             return item.toString().contains(regex);
                           }) != std::cend(data));
    }
    return false;
  }
};

#endif // DATASOURCESFILTER_H_INCLUDED
