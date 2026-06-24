#pragma once

#ifdef YOYO_FS

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace yoyo::fs {
    // Internal read file.
    std::string iread_file(const std::string& path);

    // Internal read file bytes.
    std::vector<uint8_t> iread_file_bytes(const std::string& path);

    // Internal read dir
    std::vector<std::string> iread_dir(const std::string& path);

    // read a file.
    pxs_VarT read_file(pxs_VarT args);

    // read a directory.
    pxs_VarT read_dir(pxs_VarT args);

    // Initialize the `yoyo.fs` module.
    void init(pxs_Module* yoyo);
};

#endif // YOYO_FS