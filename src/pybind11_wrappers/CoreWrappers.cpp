#include "CoreWrappers.h"

#include "pywrappers_common.h"

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
  if constexpr(row_major)
  {
    for(std::size_t i = 0; i < t.size(); i++)
    {
      dest_t[i]      = t_view[i];
      dest_values[i] = {values_view(i, 0), values_view(i, 1),
                        values_view(i, 2)};
    }
  }
  else
  {
    for(std::size_t i = 0; i < t.size(); i++)
    {
      dest_t[i]      = t_view[i];
      dest_values[i] = {values_view(0, i), values_view(1, i),
                        values_view(2, i)};
    }
  }
}

template<typename T, typename U>
void copy_scalar(py::array_t<double>& t, py::array_t<double>& values, T& dest_t,
                 U& dest_values)
{
  auto t_view      = t.unchecked<1>();
  auto values_view = values.unchecked<1>();
  for(std::size_t i = 0; i < t.size(); i++)
  {
    dest_t[i]      = t_view[i];
    dest_values[i] = values_view[i];
  }
}

template<typename T, typename U>
void copy_spectro(py::array_t<double>& t, py::array_t<double>& values,
                  T& dest_t, U& dest_values)
{
  auto t_view      = t.unchecked<1>();
  auto values_view = values.unchecked<2>();
  const auto width = values.shape(0);
  for(std::size_t i = 0; i < t.size(); i++)
  {
    dest_t[i] = t_view[i];
    for(int j = 0; j < width; j++)
    {
      dest_values[i * width + j] = values_view(j, i);
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
          py::return_value_policy::reference);

  py::class_<ScalarTimeSerie, TimeSeries::ITimeSerie>(m, "ScalarTimeSerie")
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

  py::class_<VectorTimeSerie, TimeSeries::ITimeSerie>(m, "VectorTimeSerie")
      .def(py::init<>())
      .def(py::init<std::size_t>())
      .def(py::init([](py::array_t<double> t, py::array_t<double> values) {
        assert(t.size() * 3 == values.size());
        VectorTimeSerie::axis_t _t(t.size());
        VectorTimeSerie::container_type<VectorTimeSerie::raw_value_type>
            _values(t.size());
        if(values.shape()[0] == 3 && values.shape(1) != 3)
        {
          copy_vector<decltype(_t), decltype(_values), false>(t, values, _t,
                                                              _values);
        }
        else
        {
          copy_vector(t, values, _t, _values);
        }

        return VectorTimeSerie(_t, _values);
      }))
      .def("__getitem__",
           [](VectorTimeSerie& ts, std::size_t key)
               -> VectorTimeSerie::raw_value_type& { return ts[key]; },
           py::return_value_policy::reference)
      .def("__setitem__", [](VectorTimeSerie& ts, std::size_t key,
                             VectorTimeSerie::raw_value_type value) {
        *(ts.begin() + key) = value;
      });

  py::class_<SpectrogramTimeSerie::iterator_t>(m, "SpectrogramTimeSerieItem")
      .def("__getitem__", [](SpectrogramTimeSerie::iterator_t& self,
                             std::size_t key) { return (*self)[key]; })
      .def("__setitem__",
           [](SpectrogramTimeSerie::iterator_t& self, std::size_t key,
              double value) { (*self)[key] = value; });

  py::class_<SpectrogramTimeSerie, TimeSeries::ITimeSerie>(
      m, "SpectrogramTimeSerie")
      .def(py::init<>())
      .def(py::init<const std::vector<std::size_t>>())
      .def(py::init([](py::array_t<double> t, py::array_t<double> values) {
        assert(t.size() < values.size()); // TODO check geometry
        SpectrogramTimeSerie::axis_t _t(t.size());
        SpectrogramTimeSerie::container_type<
            SpectrogramTimeSerie::raw_value_type>
            _values(values.size());
        copy_spectro(t, values, _t, _values);
        std::vector<std::size_t> shape;
        shape.push_back(values.shape(1));
        shape.push_back(values.shape(0));
        return SpectrogramTimeSerie(_t, _values, shape);
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
