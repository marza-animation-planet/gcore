#ifndef __pybindgcore_casters_h__
#define __pybindgcore_casters_h__

#include <pybind11/cast.h>
#include <gcore/list.h>
#include <gcore/path.h>
#include <gcore/string.h>

// Automatically casting gcore::String and gcore::Path to and from string
namespace PYBIND11_NAMESPACE {
namespace detail {
template <> struct type_caster<gcore::String> : string_caster<gcore::String> {};

template <> struct type_caster<gcore::Path> {
    using value_conv = make_caster<gcore::String>;

  public:
    PYBIND11_TYPE_CASTER(gcore::Path, const_name("str"));

    bool load(handle src, bool) {
        value = gcore::Path(src.cast<gcore::String>());
        return true;
    }

    static handle cast(gcore::Path src, return_value_policy, handle) {
        return PyUnicode_FromString(src.fullname('/').c_str());
    }
};

template <typename Value>
struct type_caster<gcore::List<Value>> : list_caster<gcore::List<Value>, Value> {};
template <>
struct type_caster<gcore::StringList> : list_caster<gcore::StringList, gcore::String> {};
template <>
struct type_caster<gcore::PathList> : list_caster<gcore::PathList, gcore::Path> {};
} // namespace detail
} // namespace PYBIND11_NAMESPACE

#endif  // __pybindgcore_casters_h__
