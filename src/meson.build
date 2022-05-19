sciqlopcore_moc_headers = files(
    '../include/SciQLopCore/Common/SignalWaiter.hpp',
    '../include/SciQLopCore/DataSource/IDataProvider.hpp',
    '../include/SciQLopCore/DataSource/DataSourceItemAction.hpp',
    '../include/SciQLopCore/DataSource/DataSources.hpp',
    '../include/SciQLopCore/GUI/MainWindow.hpp',
    '../include/SciQLopCore/GUI/CentralWidget.hpp',
    '../include/SciQLopCore/GUI/PlotWidget.hpp',
    '../include/SciQLopCore/GUI/TimeSyncPanel.hpp',
    '../include/SciQLopCore/GUI/PorductsTree.hpp',
    '../include/SciQLopCore/GUI/DragAndDrop.hpp',
    '../include/SciQLopCore/Common/SciQLopObject.hpp',
    '../include/SciQLopCore/Data/Pipelines.hpp'
)

sciqlopcore_moc_sources = files(
    'Common/SignalWaiter.cpp',
    'DataSource/IDataProvider.cpp',
    'DataSource/DataSourceItemAction.cpp',
    'DataSource/DataSources.cpp',
    'GUI/MainWindow.cpp',
    'GUI/PorductsTree.cpp',
    'GUI/PlotWidget.cpp',
    'GUI/TimeSyncPanel.cpp',
    'GUI/CentralWidget.cpp',
    'Common/SciQLopObject.cpp',
    'Data/Pipelines.cpp'
)

sciqlopcore_ui_sources = files('../UI/ProductsTree.ui', '../UI/mainwindow.ui')

sciqlopcore_rc_sources = files('../resources/sqpguiresources.qrc')

sciqlopcore_moc_gen = qtmod.compile_moc(
                                        sources: sciqlopcore_moc_sources,
                                        headers: sciqlopcore_moc_headers)

sciqlopcore_ui_gen = qtmod.compile_ui(sources: sciqlopcore_ui_sources)

sciqlopcore_rc_gen = qtmod.compile_resources(sources: sciqlopcore_rc_sources)


sciqlopcore_headers = files(
    '../include/SciQLopCore/Data/DataSeriesType.hpp',
    '../include/SciQLopCore/Data/DateTimeRange.hpp',
    '../include/SciQLopCore/Data/DateTimeRangeHelper.hpp',
    '../include/SciQLopCore/Data/MultiComponentTimeSerie.hpp',
    '../include/SciQLopCore/Data/ScalarTimeSerie.hpp',
    '../include/SciQLopCore/Data/SpectrogramTimeSerie.hpp',
    '../include/SciQLopCore/Data/TimeSeriesUtils.hpp',
    '../include/SciQLopCore/Data/VectorTimeSerie.hpp',
    '../include/SciQLopCore/Common/DateUtils.hpp',
    '../include/SciQLopCore/Common/debug.hpp',
    '../include/SciQLopCore/Common/MetaTypes.hpp',
    '../include/SciQLopCore/DataSource/DataSourceItem.hpp',
    '../include/SciQLopCore/DataSource/DataProviderParameters.hpp',
    '../include/SciQLopCore/DataSource/DataSourceItemMergeHelper.hpp',
    '../include/SciQLopCore/MimeTypes/MimeTypes.hpp',
    '../include/SciQLopCore/SciQLopCore.hpp',
    '../include/SciQLopCore/logging/SciQLopLogs.hpp'
)

sciqlopcore_sources = files(
    'Common/DateUtils.cpp',
    'Common/SignalWaiter.cpp',
    'DataSource/DataSourceItem.cpp',
    'DataSource/DataSourceItemMergeHelper.cpp',
    'DataSource/DataSourceItemAction.cpp',
    'DataSource/DataSources.cpp',
    'DataSource/IDataProvider.cpp',
    'GUI/MainWindow.cpp',
    'GUI/CentralWidget.cpp',
    'GUI/PorductsTree.cpp',
    'GUI/PlotWidget.cpp',
    'GUI/TimeSyncPanel.cpp',
    'SciQLopCore.cpp',
    'Common/SciQLopObject.cpp',
    'logging/SciQLopLogs.cpp',
    'Data/Pipelines.cpp'
)

sciqlopcore_lib = library('sciqlopcore',
                          sciqlopcore_moc_gen,
                          sciqlopcore_ui_gen,
                          sciqlopcore_sources,
                          sciqlopcore_rc_gen,
                          dependencies: [time_series_dep, cpp_utils_dep, SciQLopPlots_dep, qt_core, qt_widgets, qt_gui],
                          include_directories:'../include',
                          extra_files:[sciqlopcore_headers,
                                       sciqlopcore_moc_headers,
                                       sciqlopcore_ui_sources,
                                       sciqlopcore_extra_files]
                         )

sciqlopcore_dep = declare_dependency( link_with: sciqlopcore_lib,
                                      include_directories:'../include',
                                      dependencies: [time_series_dep, SciQLopPlots_dep, cpp_utils_dep, qt_core, qt_widgets, qt_gui]
                                    )
