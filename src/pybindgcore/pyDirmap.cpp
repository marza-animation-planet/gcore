#include "pybindgcore.h"
#include <gcore/dirmap.h>

void bindDirmap(py::module &m) {
    m.def_submodule("dirmap")
        .def("add_mapping", gcore::dirmap::AddMapping)
        .def("remove_mapping", gcore::dirmap::RemoveMapping)
        .def("read_mappings_from_file", gcore::dirmap::ReadMappingsFromFile)
        .def("write_mappings_to_file", gcore::dirmap::WriteMappingsToFile)
        .def("get_mapping", gcore::dirmap::Map, "path"_a, "reverse_lookup"_a = false);
}