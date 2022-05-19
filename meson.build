project(
  'SciQLopCore',
  'cpp',
  version : '0.0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17', 'default_library=static'],
  license : 'GPL3'
)

add_global_arguments('-DSCIQLOPCORE_VERSION="@0@"'.format(meson.project_version()), language : 'cpp')

qt_sdk=get_option('qt_version')
if qt_sdk=='auto'
    qt_version=run_command(find_program('qmake'), '-query', 'QT_VERSION').stdout()
    if qt_version.startswith('5.')
        qt_sdk='qt5'
    elif qt_version.startswith('6.')
        qt_sdk='qt6'
    endif
endif


qtmod = import(qt_sdk)

qt_core = dependency(qt_sdk, modules : 'Core', method:'qmake')
qt_widgets = dependency(qt_sdk, modules : 'Widgets')
qt_gui = dependency(qt_sdk, modules : 'Gui')
qt_svg = dependency(qt_sdk, modules : 'Svg')
qt_xml = dependency(qt_sdk, modules : 'Xml')
qt_network = dependency(qt_sdk, modules : 'Network')
qt_printsupport = dependency(qt_sdk, modules : 'PrintSupport')
qt_Concurrent = dependency(qt_sdk, modules : 'Concurrent')
qt_test = dependency(qt_sdk, modules : 'Test')

cpp_utils_dep = dependency('cpp_utils')
time_series_dep = dependency('TimeSeries')
SciQLopPlots_dep = dependency('SciQLopPlots')

sciqlopcore_extra_files = files('pyproject.toml', 'README.md')

subdir('src')
subdir('bindings')
subdir('tests')
