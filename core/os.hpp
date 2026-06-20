#ifdef YOYO_OS
#include <pixelscript_cpp.hpp>

namespace yoyo::os {
inline void init(pxs_Module* yoyo, int argc, char* argv[]) {
    auto yoyo_os = pxs_newmod("os");

    // Add argv
    auto argv_list = pxs::Var::new_list();
    for (int i = 0; i < argc; i++) {
        argv_list.add(pxs_newstring(argv[i]));
    }
    pxs_addvar(yoyo_os, "argv", argv_list.raw());

    pxs_add_submod(yoyo, yoyo_os);
}
};

#endif // YOYO_OS