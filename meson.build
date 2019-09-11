
catalogicpp_dep = dependency('catalogicpp', required : true, fallback:['catalogicpp','catalogicpp_dep'])
pybind11_dep = dependency('pybind11', required : true, fallback:['pybind11','pybind11_dep'])
timeseries_dep = dependency('TimeSeries', required : true, fallback:['TimeSeries','time_series_dep'])
cpp_utils_dep = dependency('cpp_utils', fallback:['cpp_utils','cpp_utils_dep'])

core_moc_headers = [
 './include/Common/spimpl.h',
 './include/Common/DateUtils.h',
 './include/Common/MimeTypesDef.h',
 './include/Common/SignalWaiter.h',
 './include/Common/debug.h',
 './include/Common/MetaTypes.h',
 './include/Common/SortUtils.h',
 './include/Data/DateTimeRangeHelper.h',
 './include/Data/ScalarTimeSerie.h',
 './include/Data/DateTimeRange.h',
 './include/Data/DataProviderParameters.h',
 './include/Data/TimeSeriesUtils.h',
 './include/Data/VectorTimeSerie.h',
 './include/Data/SqpIterator.h',
 './include/Data/IDataProvider.h',
 './include/Data/SpectrogramTimeSerie.h',
 './include/Data/MultiComponentTimeSerie.h',
 './include/Data/DataSeriesType.h',
 './include/CoreGlobal.h',
 './include/Network/NetworkController.h',
 './include/Network/Response.h',
 './include/Network/Downloader.h',
 './include/Settings/SqpSettingsDefs.h',
 './include/Settings/ISqpSettingsBindable.h',
 './include/DataSource/DataSourceController.h',
 './include/DataSource/DataSourceItem.h',
 './include/DataSource/DataSourceItemAction.h',
 './include/DataSource/DataSourceItemMergeHelper.h',
 './include/Time/TimeController.h',
 './include/PluginManager/PluginManager.h',
 './include/Version.h',
 './include/Catalogue/CatalogueController.h',
 './include/Plugin/IPlugin.h',
 './include/Variable/VariableModel2.h',
 './include/Variable/VariableController2.h',
 './include/Variable/Variable2.h',
 './include/Variable/VariableSynchronizationGroup2.h',
 './include/Variable/private/VCTransaction.h']



core_moc_sources = ['src/Network/Downloader.cpp']


core_moc_files = qt5.preprocess(moc_headers : core_moc_headers, moc_sources: core_moc_sources)

core_sources = ['./src/Common/MimeTypesDef.cpp',
 './src/Common/SignalWaiter.cpp',
 './src/Common/DateUtils.cpp',
 './src/Network/Downloader.cpp',
 './src/Network/NetworkController.cpp',
 './src/Settings/SqpSettingsDefs.cpp',
 './src/DataSource/DataSourceItemAction.cpp',
 './src/DataSource/DataSourceItemMergeHelper.cpp',
 './src/DataSource/DataSourceItem.cpp',
 './src/DataSource/DataSourceController.cpp',
 './src/Time/TimeController.cpp',
 './src/PluginManager/PluginManager.cpp',
 './src/Version.cpp',
 './src/Catalogue/CatalogueController.cpp',
 './src/Variable/VariableSynchronizationGroup2.cpp',
 './src/Variable/Variable2.cpp',
 './src/Variable/VariableController2.cpp',
 './src/Variable/VariableModel2.cpp']


core_inc = include_directories(['include', 'include/Plugin'])

sciqlop_core_lib = library('sciqlopcore',
                       core_sources,
                       core_moc_files,
                       cpp_args : '-DCORE_LIB',
                       include_directories : core_inc,
                       dependencies : [qt5core, qt5network, catalogicpp_dep, pybind11_dep, timeseries_dep, cpp_utils_dep],
                       install : true
                       )


sciqlop_core = declare_dependency(link_with : sciqlop_core_lib,
                                include_directories : core_inc,
                                dependencies : [qt5core, qt5network, catalogicpp_dep, pybind11_dep, timeseries_dep, cpp_utils_dep])

pymod = import('python')
python3 = pymod.find_installation('python3')

pysciqlopcore_srcs = [
 './src/pybind11_wrappers/CatalogWrappers.cpp',
 './src/pybind11_wrappers/QtWrappers.cpp',
 './src/pybind11_wrappers/CoreWrappers.cpp'
]

python3.extension_module('pysciqlopcore', './src/pybind11_wrappers/CoreWrappers.cpp', 
                         dependencies: [sciqlop_core], 
                         install: true
                        )


subdir('tests')

