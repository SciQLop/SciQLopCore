/*------------------------------------------------------------------------------
-- This file is a part of the SciQLop Software
-- Copyright (C) 2022, Plasma Physics Laboratory - CNRS
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
-------------------------------------------------------------------------------*/
/*-- Author : Alexis Jeandet
-- Mail : alexis.jeandet@member.fsf.org
----------------------------------------------------------------------------*/
#include "SciQLopCore/GUI/PorductsTree.hpp"

#include "SciQLopCore/DataSource/DataSources.hpp"
#include "SciQLopCore/SciQLopCore.hpp"
#include "ui_ProductsTree.h"

#include <QAction>
#include <QCompleter>


ProductsTree::ProductsTree(QWidget* parent)
    : QWidget{parent}, ui{new Ui::ProductsTree}
{
  ui->setupUi(this);
  m_model_proxy.setSourceModel(&SciQLopCore::dataSources());
  ui->treeView->setModel(&m_model_proxy);
  ui->treeView->setDragEnabled(true);
  m_model_proxy.setFilterRole(Qt::DisplayRole);
  m_model_proxy.setRecursiveFilteringEnabled(true);
  m_model_proxy.setAutoAcceptChildRows(true);

  connect(ui->filterLineEdit, &QLineEdit::textChanged, &m_model_proxy,[this](const QString& filter)
  {
      m_model_proxy.setFilterFixedString(filter);
  });

  QAction* expandAll   = new QAction("Expand all");
  QAction* collapseAll = new QAction("Collapse all");
  ui->treeView->addAction(expandAll);
  ui->treeView->addAction(collapseAll);
  ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
  connect(expandAll, &QAction::triggered, this,
          [treeView = ui->treeView](bool checked) {
            (void)checked;
            treeView->expandAll();
          });
  connect(collapseAll, &QAction::triggered, this,
          [treeView = ui->treeView](bool checked) {
            (void)checked;
            treeView->collapseAll();
          });

  QCompleter* completer = new QCompleter(this);
  completer->setModel(SciQLopCore::dataSources().completionModel());
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  ui->filterLineEdit->setCompleter(completer);
}

ProductsTree::~ProductsTree() { delete ui; }

void ProductsTree::updateTreeWidget() noexcept {}
