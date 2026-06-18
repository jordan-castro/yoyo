#include "yoyo.h"

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>

#include <string>
#include <iostream>

// TODO: Add VFS for compile mode.
#include <fstream>
#include <filesystem>

// TODO: Add VFS for compile mode.
pxs_VarT yoyo_load_file(const char* file_path) {
    if (!file_path) {
        return pxs_newnull();
    }
    #ifdef YOYO_DEBUG
    std::cout << "IMPORTING " << file_path << std::endl;
    #endif

    // read file
    std::ifstream file(file_path);
    std::string contents;
    std::string line;
    while(std::getline(file, line)) {
        contents += line + "\n";
    }
    file.close();

    return pxs_newstring(contents.c_str());
}

pxs_VarT yoyo_read_dir(const char* dir_path) {
    if (!dir_path) {
        return pxs_newnull();
    }

    std::filesystem::path dpath(dir_path);
    if (!std::filesystem::exists(dpath) || !std::filesystem::is_directory(dpath)) {
        return pxs_newnull();
    }

    auto list = pxs_newlist();
    for (const auto& entry : std::filesystem::directory_iterator(dpath)) {
        auto res = entry.path().filename();
        pxs_listadd(list, pxs_newstring(res.string().c_str()));
    }

    return list;
}

// Sets up module reading
void setup_module_loading() {
    pxs_set_filereader(yoyo_load_file);
    pxs_set_dirreader(yoyo_read_dir);
}

#ifdef YOYO_APP
// For repl only. It will publish the yoyo module and other helpful modules.
void setup_repl(pxs_Runtime rt) {
    std::string code;
    switch(rt) {
        case pxs_Runtime::pxs_Lua: 
            code = "yoyo = require('yoyo')";
            break;
        case pxs_Runtime::pxs_Python:
            code = "import yoyo";
            break;
        case pxs_Runtime::pxs_JavaScript:
            code = "import * as yoyo from 'yoyo'; globalThis.yoyo = yoyo;";
            break;
        case pxs_Runtime::pxs_Wren:
            return;
    }
    auto val = pxs_exec(rt, code.c_str(), "<repl>");
    #ifdef YOYO_DEBUG
    pxs::Var val_wrapper = pxs::Var(val);
    std::cout << val_wrapper.debug() << std::endl;
    #endif // YOYO_DEBUG
    pxs_freevar(val);
}
#endif // YOYO_APP

extern "C" {

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

    std::cout << msg << std::endl;
    return pxs_newnull();
}

#endif // YOYO_CORE

void yoyo_init() {
    pxs_initialize();
    setup_module_loading();
    auto yoyo = pxs_newmod("yoyo");

    #ifdef YOYO_CORE
    pxs_addfunc(yoyo, "print", yoyo_print);
    #endif // YOYO_CORE
    
    pxs_addmod(yoyo);
}

void yoyo_stop() {
    // #ifdef YOYO_DEBUG
    // char* state = pxs_debugstate(pxs_Runtime::pxs_JavaScript);
    // std::cout << state << std::endl;
    // pxs_freestr(state);
    // #endif // YOYO_DEBUG
    pxs_finalize();
}

#ifdef YOYO_APP
void yoyo_repl(int runtime) {
    // Convert runtime to a pxs_Runtime enum
    pxs_Runtime rt = static_cast<pxs_Runtime>(runtime);

    // Setup runtime.
    setup_repl(rt);
    pxs::Var rt_var = pxs::Var(pxs_newint(rt));
    rt_var.set_owned(true);

    while(true) {
        // Get input from user
        std::string input;
        std::cout << "> ";
        if (!std::getline(std::cin, input) || input == "quit") {
            break;
        }

        if (input.empty()) {
            continue;
        }

        // eVal
        pxs::Var result = pxs::Var(rt_var.raw(), pxs_exec(rt, input.c_str(), "<repl>"), true);
        if (result.is(pxs_Exception)) {
            std::cout << result.get_string() << std::endl;
        }
    }
}

const char* yoyo_run(int runtime, const char* code, const char* file_name) {
    pxs_Runtime rt = static_cast<pxs_Runtime>(runtime);

    pxs::Var runtime_var = pxs::Var(pxs_newint(rt));
    runtime_var.set_owned(true);
    pxs::Var result = pxs::Var(runtime_var.raw(), pxs_exec(rt, code, file_name), true);

    return result.to_string().c_str();
}
#endif // YOYO_APP
}