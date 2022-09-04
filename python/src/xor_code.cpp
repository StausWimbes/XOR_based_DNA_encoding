#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include "xor_code/settings.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

namespace xor_code {
    std::vector<std::string> encode_helper(const std::string &filename, const py::array_t<bool> &data) {
        std::vector<std::string> result;

        // TODO

        return result;
    }

    py::array_t<bool> decode_helper(const std::string &filename, const std::vector<std::string> &strands) {
        std::vector<bool> result;

        // TODO

        return py::cast(result);
    }


    PYBIND11_MODULE(xor_code_python, m) {
        m.doc() = R"pbdoc(
            Python bindings for the implementation of the xor based DNA Encoding created by Bornholt et al..
            )pbdoc";

        m.def("encode", &encode_helper, "Encode the given data.", "filename"_a, "data"_a);
        m.def("decode", &decode_helper, "Decode the given strands.", "filename"_a, "strands"_a);


        m.def("get_max_strand_length", &getMaxStrandLength, R"pbdoc(
            Returns the maximum length of a strand measured in nucleotides.
            )pbdoc");

        m.def("set_max_strand_length", &setMaxStrandLength, R"pbdoc(
            Sets the maximum length of a strand measured in nucleotides to the given parameter.
            )pbdoc", "length"_a);


        m.def("get_payload_byte_length", &getPayloadByteLength, R"pbdoc(
            Returns the amount of bytes a single payload contains.
            )pbdoc");


        m.def("get_xor_size", &getXorSize, R"pbdoc(
            Returns the amount of strands that that are used to create a single xor strand.
            )pbdoc");

        m.def("set_xor_size", &setXorSize, R"pbdoc(
            Sets the amount of strands that that are used to create a single xor strand to the given parameter.
            )pbdoc", "size"_a);


        m.def("get_xor_step", &getXorStep, R"pbdoc(
            Returns the step size for the creation of the xor strands.
            If the strand with index i is the first strand used to create a xor strand then the first strand for the next xor strand will have index i + step size.
            )pbdoc");

        m.def("set_xor_step", &setXorStep, R"pbdoc(
            Sets the step size for the creation of the xor strands to the given parameter.
            If the strand with index i is the first strand used to create a xor strand then the first strand for the next xor strand will have index i + step size.
            )pbdoc", "step"_a);
    }
}
