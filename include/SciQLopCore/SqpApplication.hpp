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
#pragma once


#include <QAction>
#include <QApplication>
#include <QLoggingCategory>
#include <QMenuBar>
#include <QProxyStyle>
#include <QStyleOption>
#include <QWidget>
#include <QWidgetAction>
#include <memory>


Q_DECLARE_LOGGING_CATEGORY(LOG_SqpApplication)

#if defined(sqpApp)
#undef sqpApp
#endif
#define sqpApp (static_cast<SqpApplication*>(QCoreApplication::instance()))

class DataSources;


/* stolen from here https://forum.qt.io/topic/90403/show-tooltip-immediatly/6 */
class MyProxyStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    int styleHint(StyleHint hint, const QStyleOption* option = nullptr,
        const QWidget* widget = nullptr, QStyleHintReturn* returnData = nullptr) const override
    {
        if (hint == QStyle::SH_ToolButton_PopupDelay && widget
            /*&& widget->inherits(QWidgetAction::staticMetaObject.className())*/)
        {
            return 0;
        }

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

/**
 * @brief The SqpApplication class aims to make the link between SciQlop
 * and its plugins. This is the intermediate class that SciQlop has to use
 * in the way to connect a data source. Please first use load method to initialize
 * a plugin specified by its metadata name (JSON plugin source) then others specifics
 * method will be able to access it.
 * You can load a data source driver plugin then create a data source.
 */

class SqpApplication : public QApplication
{
    Q_OBJECT
public:
    explicit SqpApplication(int& argc, char** argv);
    ~SqpApplication() override;
    void initialize();

    /// Accessors for the differents sciqlop controllers
    //DataSourceController& dataSourceController() noexcept;
    DataSources& dataSources() noexcept;

private:
    class SqpApplicationPrivate;
    std::unique_ptr<SqpApplicationPrivate> impl;
};

inline SqpApplication* SqpApplication_ctor()
{
    static int argc;
    static char** argv;
    return new SqpApplication(argc, argv);
}
