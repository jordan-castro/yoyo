#ifdef YOYO_PXS

#include <pixelscript_cpp.hpp>
#include "debug.hpp"

// ipxs = internal pxs module.
// since pixelscript_cpp already uses `pxs` as a module.

namespace yoyo::ipxs {
    // Execute code
    // Null means it was executed succesfully.
    inline pxs_VarT exec(pxs_VarT args) {
        PXS_ARGC_EQ(3); // runtime option, code, code name
        // Get runtime
        auto runtime = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(runtime.raw(), pxs_Int64);
        pxs_Runtime rt = static_cast<pxs_Runtime>(runtime.get_int());
        // Get code
        auto code = pxs::Var::from_args(args, 1);
        PXS_ARG_IS_TYPE(code.raw(), pxs_String);
        // Get code name
        auto code_name = pxs::Var::from_args(args, 2);
        PXS_ARG_IS_TYPE(code_name.raw(), pxs_String);

        // Execute some code yo!
        // And return the result.
        auto res = pxs::Var(pxs_exec(rt, code.get_string().c_str(), code_name.get_string().c_str()), true);
        if (res.is(pxs_Exception)) {
            return pxs_newstring(res.get_string().c_str());
        }
        return pxs_newnull();
    }

    inline void init(pxs_Module* yoyo) {
        auto pxs = pxs_newmod("pxs");

        // Define runtime variables
        pxs_addvar(pxs, "PYTHON", pxs_newint(pxs_Runtime::pxs_Python));
        pxs_addvar(pxs, "LUA", pxs_newint(pxs_Runtime::pxs_Lua));
        pxs_addvar(pxs, "JS", pxs_newint(pxs_Runtime::pxs_JavaScript));

        // Define exec and TODO: eval
        pxs_addfunc(pxs, "exec", exec);

        pxs_add_submod(yoyo, pxs);
    }
};

#endif // YOYO_PXS

// #pragma once

// #ifdef YOYO_PXS

// #include <pixelscript.h>

// namespace yoyo::pxs {
//     void init(pxs_Module* yoyo);
// };

// #endif // YOYO_PXS