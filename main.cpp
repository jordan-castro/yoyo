#include "pixelscript/pixelscript.h"
#include <string>
#include <iostream>

#ifdef YOYO
/**
 * yoyo
 */
pxs_VarT yoyo_print(pxs_VarT args) {
    // Go through args
    int argc = pxs_listlen(args);
    std::string result;
    auto runtime = pxs_listget(args, 0);
    for (int i = 1; i < argc; i++) {
        auto val = pxs_listget(args, i);
        auto str_val = pxs_tostring(runtime, val);
        auto str = pxs_getstring(str_val);
        pxs_freevar(str_val);
        result += std::string(str);

        pxs_freestr(str);
    }

    std::cout << result << "\n" << std::endl;
}

#endif // YOYO

int main() {
    pxs_initialize();

    // Initialize any modules here.
    #ifdef YOYO
        auto yoyo = pxs_newmod("yoyo");
        pxs_addfunc(yoyo, "print", yoyo_print);
        pxs_addmod(yoyo);
    #endif // YOYO

    auto result = pxs_exec(PXS_RUNTIME, PXS_CODE, PXS_FILE_NAME);
    if (!pxs_varis(result, pxs_Null)) {
        auto error = pxs_getstring(result);
        std::cerr << error << "\n" << std::endl;
        pxs_freestr(error);
        pxs_freevar(result);
        return 1;
    }
    pxs_freevar(result);
    
    pxs_finalize();
    return 0;
}