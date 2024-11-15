#include "pybindgcore.h"
#include <gcore/argparser.h>

void bindArgparser(py::module &m) {
    py::class_<gcore::FlagDesc>(m, "FlagDesc")
        .def(py::init<>())
        .def(py::init<ecolo::Environment>())
        .def("__str__", &ecolo::Environment::toString);
}