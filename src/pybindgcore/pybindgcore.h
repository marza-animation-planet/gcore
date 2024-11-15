#ifndef __pybindgcore_pybindgcore_h__
#define __pybindgcore_pybindgcore_h__

#include <pybindgcore/casters.h>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;

// void bindStringconv(py::module &m);
void bindArgparser(py::module &m);
void bindDirmap(py::module &m);
void bindEnv(py::module &m);
void bindLog(py::module &m);
void bindMD5(py::module &m);
void bindPath(py::module &m);
void bindPerflog(py::module &m);
void bindPlist(py::module &m);
void bindXML(py::module &m);
void bindPipe(py::module &m);

#endif  // __pybindgcore_pybindgcore_h__