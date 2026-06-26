#pragma once

#include <pixelscript.h>

// Helpful utilities for PXS support.
namespace yoyo::utils::pxs {
    template<typename T>
    pxs_VarT enum_to_int(T e) {
        return pxs_newint(static_cast<int>(e));
    }
};