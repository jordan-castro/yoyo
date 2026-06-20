#include "yoyo.hpp"
#include "os.hpp"
#include "pxs.hpp"
#include "fs.hpp"
#include "shell.hpp"

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>

#include <string>
#include <iostream>

// TODO: Add VFS for compile mode.
#include <fstream>
#include <filesystem>

pxs_VarT yoyo_load_file(const char* file_path) {
    if (!file_path) {
        return pxs_newnull();
    }
    // #ifdef YOYO_DEBUG
    // std::cout << "IMPORTING " << file_path << std::endl;
    // #endif

    auto contents = yoyo::fs::iread_file(file_path);

    return pxs_newstring(contents.c_str());
}

pxs_VarT yoyo_read_dir(const char* dir_path) {
    if (!dir_path) {
        return pxs_newnull();
    }

    auto contents = yoyo::fs::iread_dir(std::string(dir_path));
    auto list = pxs_newlist();
    for (const auto& entry : contents) {
        pxs_listadd(list, pxs_newstring(entry.c_str()));
    }

    return list;
}

// Sets up module reading
void setup_module_loading() {
    pxs_set_filereader(yoyo_load_file);
    pxs_set_dirreader(yoyo_read_dir);
}

#ifdef YOYO_APP
#endif // YOYO_APP

#ifdef YOYO_CORE

/// `yoyo.print`
pxs_VarT yoyo_print(pxs_VarT args) {
    std::string msg;
    pxs::Var arg_wrapper = pxs::Var(pxs_listget(args, 0), args);
    int argc = arg_wrapper.list_len();
    for (int i = 1; i < argc; i++) {
        auto var = arg_wrapper.list_get(i).to_string();
        msg += var;
        if (i < argc - 1) {
            msg += " ";
        }
    }

    std::cout << msg;
    return pxs_newnull();
}

/// `yoyo.println`
pxs_VarT yoyo_println(pxs_VarT args) {
    pxs_freevar(yoyo_print(args));
    std::cout << std::endl;
    return pxs_newnull();
}

/// `yoyo.readln`
pxs_VarT yoyo_readln(pxs_VarT args) {
    std::string line;
    std::getline(std::cin, line);

    return pxs_newstring(line.c_str());
}

#endif // YOYO_CORE

void yoyo::init(int argc, char* argv[]) {
    pxs_initialize();
    setup_module_loading();
    auto yoyo = pxs_newmod("yoyo");

    #ifdef YOYO_CORE
    pxs_addfunc(yoyo, "print", yoyo_print);
    pxs_addfunc(yoyo, "println", yoyo_println);
    pxs_addfunc(yoyo, "readln", yoyo_readln);
    #endif // YOYO_CORE

    #ifdef YOYO_OS
    yoyo::os::init(yoyo, argc, argv);
    #endif // YOYO_OS

    #ifdef YOYO_PXS
    yoyo::ipxs::init(yoyo);
    #endif // YOYO_PXS

    #ifdef YOYO_FS
    yoyo::fs::init(yoyo);
    #endif // YOYO_FS

    #ifdef YOYO_SHELL
    yoyo::shell::init(yoyo);
    #endif // YOYO_SHELL
    
    pxs_addmod(yoyo);
}

void yoyo::stop() {
    // #ifdef YOYO_DEBUG
    // char* state = pxs_debugstate(pxs_Runtime::pxs_JavaScript);
    // std::cout << state << std::endl;
    // pxs_freestr(state);
    // #endif // YOYO_DEBUG
    pxs_finalize();
}

// #ifdef YOYO_APP
// void yoyo::repl(pxs_Runtime rt) {
//     // Setup runtime.
//     setup_repl(rt);
//     pxs::Var rt_var = pxs::Var(pxs_newint(rt));
//     rt_var.set_owned(true);

//     while(true) {
//         // Get input from user
//         std::string input;
//         std::cout << "> ";
//         if (!std::getline(std::cin, input) || input == "quit") {
//             break;
//         }

//         if (input.empty()) {
//             continue;
//         }

//         // eVal
//         pxs::Var result = pxs::Var(rt_var.raw(), pxs_exec(rt, input.c_str(), "<repl>"), true);
//         if (result.is(pxs_Exception)) {
//             std::cout << result.get_string() << std::endl;
//         }
//     }
// }

// #endif // YOYO_APP

pxs::Var yoyo::run(pxs_Runtime runtime, const std::string& code, const std::string& file_name) {
    return pxs::Var(nullptr, pxs_exec(runtime, code.c_str(), file_name.c_str()), true);
}