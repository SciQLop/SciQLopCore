#include "CoreWrappers.h"

#include "pywrappers_common.h"

#include <Common/debug.h>
#include <Data/DataSeriesType.h>
#include <Data/IDataProvider.h>
#include <Network/Downloader.h>
#include <Time/TimeController.h>
#include <Variable/Variable2.h>
#include <Variable/VariableController2.h>
#include <pybind11/chrono.h>
#include <pybind11/embed.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <sstream>
#include <string>

namespace py = pybind11;
using namespace std::chrono;

template<typename T, typename U, bool row_major = true>
void copy_vector(py::array_t<double>& t, py::array_t<double>& values, T& dest_t,
                 U& dest_values)
{
  auto t_view      = t.unchecked<1>();
  auto values_view = values.unchecked<2>();
  for(std::size_t i = 0; i < t.size(); i++)
  {
    dest_t[i]      = t_view[i];
    dest_values[i] = {values_view(i, 0), values_view(i, 1), values_view(i, 2)};
  }
}

template<typename T, typename U>
void copy_scalar(py::array_t<double>& t, py::array_t<double>& values, T& dest_t,
                 U& dest_values)
{
  auto t_view = t.unchecked<1>();
  if(values.ndim() == 1)
  {
    auto values_view = values.unchecked<1>();
    for(std::size_t i = 0; i < t.size(); i++)
    {
      dest_t[i]      = t_view[i];
      dest_values[i] = values_view[i];
    }
  }
  else if(values.ndim() == 2 && values.shape(1) == 1)
  {
    auto values_view = values.unchecked<2>();
    for(std::size_t i = 0; i < t.size(); i++)
    {
      dest_t[i]      = t_view[i];
      dest_values[i] = values_view(i, 0);
    }
  }
}
template<typename T, typename U>
void copy_multicomp(py::array_t<double>& t, py::array_t<double>& values,
                    T& dest_t, U& dest_values)
{
  auto t_view      = t.unchecked<1>();
  auto values_view = values.unchecked<2>();
  const auto width = values.shape(1);
  for(std::size_t i = 0; i < t.size(); i++)
  {
    dest_t[i] = t_view[i];
    for(int j = 0; j < width; j++)
    {
      dest_values[i * width + j] = values_view(i, j);
    }
  }
}

template<typename T, typename U>
void copy_spectro(py::array_t<double>& t, py::array_t<double>& y,
                  py::array_t<double>& values, T& dest_t, T& dest_y,
                  U& dest_values)
{
  auto t_view      = t.unchecked<1>();
  auto y_view      = y.unchecked<1>();
  auto values_view = values.unchecked<2>();
  const auto width = values.shape(1);
  for(std::size_t i = 0; i < y.size(); i++)
  {
    dest_y[i] = y_view[i];
  }
  for(std::size_t i = 0; i < t.size(); i++)
  {
    dest_t[i] = t_view[i];
    for(int j = 0; j < width; j++)
    {
      dest_values[i * width + j] = values_view(i, j);
    }
  }
}

PYBIND11_MODULE(pysciqlopcore, m)
{
  pybind11::bind_vector<std::vector<double>>(m, "VectorDouble");

  py::enum_<DataSeriesType>(m, "DataSeriesType")
      .value("SCALAR", DataSeriesType::SCALAR)
      .value("SPECTROGRAM", DataSeriesType::SPECTROGRAM)
      .value("VECTOR", DataSeriesType::VECTOR)
      .value("MULTICOMPONENT", DataSeriesType::MULTICOMPONENT)
      .value("NONE", DataSeriesType::NONE)
      .export_values();

  py::class_<Response>(m, "Response")
      .def("status_code", &Response::status_code);

  py::class_<Downloader>(m, "Downloader")
      .def_static("get", Downloader::get)
      .def_static("getAsync", Downloader::getAsync)
      .def_static("downloadFinished", Downloader::downloadFinished);

  py::class_<IDataProvider, std::shared_ptr<IDataProvider>>(m, "IDataProvider");

  py::class_<TimeSeries::ITimeSerie, std::shared_ptr<TimeSeries::ITimeSerie>>(
      m, "ITimeSerie")
      .def_property_readonly(
          "size", [](const TimeSeries::ITimeSerie& ts) { return ts.size(); })
      .def("__len__",
           [](const TimeSeries::ITimeSerie& ts) { return ts.size(); })
      .def_property_readonly(
          "shape", [](const TimeSeries::ITimeSerie& ts) { return ts.shape(); })
      .def_property_readonly(
          "t",
          [](TimeSeries::ITimeSerie& ts) -> decltype(ts.axis(0))& {
            return ts.axis(0);
          },
          py::return_value_policy::reference)
      .def(
          "axis",
          [](TimeSeries::ITimeSerie& ts, unsigned int index)
              -> decltype(ts.axis(0))& { return ts.axis(index); },
          py::return_value_policy::reference);

  py::class_<ScalarTimeSerie, TimeSeries::ITimeSerie,
             std::shared_ptr<ScalarTimeSerie>>(m, "ScalarTimeSerie")
      .def(py::init<>())
      .def(py::init<std::size_t>())
      .def(py::init([](py::array_t<double> t, py::array_t<double> values) {
        assert(t.size() == values.size());
        ScalarTimeSerie::axis_t _t(t.size());
        ScalarTimeSerie::axis_t _values(t.size());
        copy_scalar(t, values, _t, _values);
        return ScalarTimeSerie(_t, _values);
      }))
      .def("__getitem__",
           [](ScalarTimeSerie& ts, std::size_t key) { return ts[key]; })
      .def("__setitem__", [](ScalarTimeSerie& ts, std::size_t key,
                             double value) { *(ts.begin() + key) = value; });

  py::class_<VectorTimeSerie::raw_value_type>(m, "vector")
      .def(py::init<>())
      .def(py::init<double, double, double>())
      .def("__repr__", __repr__<VectorTimeSerie::raw_value_type>)
      .def_readwrite("x", &VectorTimeSerie::raw_value_type::x)
      .def_readwrite("y", &VectorTimeSerie::raw_value_type::y)
      .def_readwrite("z", &VectorTimeSerie::raw_value_type::z);

  py::class_<VectorTimeSerie, TimeSeries::ITimeSerie,
             std::shared_ptr<VectorTimeSerie>>(m, "VectorTimeSerie")
      .def(py::init<>())
      .def(py::init<std::size_t>())
      .def(py::init([](py::array_t<double> t, py::array_t<double> values) {
        assert(t.size() * 3 == values.size());
        VectorTimeSerie::axis_t _t(t.size());
        VectorTimeSerie::data_t _values(t.size());
        copy_vector(t, values, _t, _values);
        return VectorTimeSerie(_t, _values);
      }))
      .def(
          "__getitem__",
          [](VectorTimeSerie& ts, std::size_t key)
              -> VectorTimeSerie::raw_value_type& { return ts[key]; },
          py::return_value_policy::reference)
      .def("__setitem__", [](VectorTimeSerie& ts, std::size_t key,
                             VectorTimeSerie::raw_value_type value) {
        *(ts.begin() + key) = value;
      });

  py::class_<MultiComponentTimeSerie::iterator_t>(m,
                                                  "MultiComponentTimeSerieItem")
      .def("__getitem__", [](MultiComponentTimeSerie::iterator_t& self,
                             std::size_t key) { return (*self)[key]; })
      .def("__setitem__",
           [](MultiComponentTimeSerie::iterator_t& self, std::size_t key,
              double value) { (*self)[key] = value; });

  py::class_<MultiComponentTimeSerie, TimeSeries::ITimeSerie,
             std::shared_ptr<MultiComponentTimeSerie>>(
      m, "MultiComponentTimeSerie")
      .def(py::init<>())
      .def(py::init<const std::vector<std::size_t>>())
      .def(py::init([](py::array_t<double> t, py::array_t<double> values) {
        assert((t.size() < values.size()) |
               (t.size() == 0)); // TODO check geometry
        MultiComponentTimeSerie::axis_t _t(t.size());
        MultiComponentTimeSerie::data_t _values(values.size());
        copy_multicomp(t, values, _t, _values);
        std::vector<std::size_t> shape;
        shape.push_back(values.shape(0));
        shape.push_back(values.shape(1));
        return MultiComponentTimeSerie(_t, _values, shape);
      }))
      .def("__getitem__",
           [](MultiComponentTimeSerie& ts,
              std::size_t key) -> MultiComponentTimeSerie::iterator_t {
             return ts.begin() + key;
           });

  py::class_<SpectrogramTimeSerie::iterator_t>(m, "SpectrogramTimeSerieItem")
      .def("__getitem__", [](SpectrogramTimeSerie::iterator_t& self,
                             std::size_t key) { return (*self)[key]; })
      .def("__setitem__",
           [](SpectrogramTimeSerie::iterator_t& self, std::size_t key,
              double value) { (*self)[key] = value; });

  py::class_<SpectrogramTimeSerie, TimeSeries::ITimeSerie,
             std::shared_ptr<SpectrogramTimeSerie>>(m, "SpectrogramTimeSerie")
      .def(py::init<>())
      .def(py::init<const std::vector<std::size_t>>())
      .def(py::init([](py::array_t<double> t, py::array_t<double> y,
                       py::array_t<double> values, double min_sampling,
                       double max_sampling, bool y_is_log) {
        if(t.size() >= values.size() and t.size() != 0)
          SCIQLOP_ERROR(decltype(py::self), "Doesn't look like a Spectrogram");
        if(y.size() != values.shape(1))
          SCIQLOP_ERROR(decltype(py::self),
                        "Y axis size and data shape are incompatible");
        SpectrogramTimeSerie::axis_t _t(t.size());
        SpectrogramTimeSerie::axis_t _y(y.size());
        SpectrogramTimeSerie::data_t _values(values.size());
        copy_spectro(t, y, values, _t, _y, _values);
        std::vector<std::size_t> shape;
        shape.push_back(values.shape(0));
        shape.push_back(values.shape(1));
        return SpectrogramTimeSerie(std::move(_t), std::move(_y),
                                    std::move(_values), shape, min_sampling,
                                    max_sampling, y_is_log);
      }))
      .def("__getitem__",
           [](SpectrogramTimeSerie& ts,
              std::size_t key) -> SpectrogramTimeSerie::iterator_t {
             return ts.begin() + key;
           });

  py::class_<Variable2, std::shared_ptr<Variable2>>(m, "Variable2")
      .def(py::init<const QString&>())
      .def_property("name", &Variable2::name, &Variable2::setName)
      .def_property_readonly("range", &Variable2::range)
      .def_property_readonly("nbPoints", &Variable2::nbPoints)
      .def_property_readonly(
          "data",
          [](Variable2& var) -> std::shared_ptr<TimeSeries::ITimeSerie> {
            return var.data();
          })
      .def("set_data",
           [](Variable2& var, std::vector<TimeSeries::ITimeSerie*> ts_list,
              const DateTimeRange& range) { var.setData(ts_list, range); })
      .def("__len__", &Variable2::nbPoints)
      .def("__repr__", __repr__<Variable2>);

  py::class_<DateTimeRange>(m, "SqpRange")
      //.def("fromDateTime", &DateTimeRange::fromDateTime,
      // py::return_value_policy::move)
      .def(py::init([](double start, double stop) {
        return DateTimeRange{start, stop};
      }))
      .def(py::init(
          [](system_clock::time_point start, system_clock::time_point stop) {
            double start_ =
                0.001 *
                duration_cast<milliseconds>(start.time_since_epoch()).count();
            double stop_ =
                0.001 *
                duration_cast<milliseconds>(stop.time_since_epoch()).count();
            return DateTimeRange{start_, stop_};
          }))
      .def_property_readonly("start",
                             [](const DateTimeRange& range) {
                               return system_clock::from_time_t(range.m_TStart);
                             })
      .def_property_readonly("stop",
                             [](const DateTimeRange& range) {
                               return system_clock::from_time_t(range.m_TEnd);
                             })
      .def("__repr__", __repr__<DateTimeRange>);
}
