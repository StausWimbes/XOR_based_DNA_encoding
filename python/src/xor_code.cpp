#include <pybind11/pybind11.h>
#include "../../cpp/include/xor_code/settings.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

namespace xor_code {
    PYBIND11_MODULE(xor_code_python, m) {
        m.doc() = R"pbdoc(
            Python bindings for the implementation of the xor based DNA Encoding created by Bornholt et al..
            )pbdoc";


        m.def("get_max_strand_length", &getMaxStrandLength, R"pbdoc(
            Returns the maximum length of a strand measured in nucleotides.
            )pbdoc");

        m.def("set_max_strand_length", &setMaxStrandLength, R"pbdoc(
            Sets the maximum length of a strand measured in nucleotides to the given parameter.
            )pbdoc", "length"_a);
    }
}
