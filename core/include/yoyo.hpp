#pragma once

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>

#include <cstdint>
#include <string>

namespace yoyo {
    // Initialize the yoyo module.
    void init(int argc, char* argv[]);
    // Stop the yoyo runtime.
    void stop();
    // Run source code
    pxs::Var run(pxs_Runtime runtime, const std::string& code, const std::string& file_name);
};
