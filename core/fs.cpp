#ifdef YOYO_FS

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "fs.hpp"

namespace yoyo::fs {
    // Internal read file.
    std::string iread_file(const std::string& path) {
        // TODO: Add VFS for compile mode.
        // read file
        std::ifstream file(path);
        std::string contents;
        std::string line;
        while (std::getline(file, line)) {
            contents += line + "\n";
        }
        file.close();

        return contents;
    }

    // Internal read dir
    std::vector<std::string> iread_dir(const std::string& path) {
        std::filesystem::path dpath(path);
        if (!std::filesystem::exists(dpath) || !std::filesystem::is_directory(dpath)) {
            return {};
        }

        std::vector<std::string> list;
        // auto list = pxs_newlist();
        for (const auto& entry : std::filesystem::directory_iterator(dpath)) {
            auto res = entry.path().filename();
            list.push_back(res.string());
        }

        return list;
    }

    // read a file.
    pxs_VarT read_file(pxs_VarT args) {
        PXS_ARGC_EQ(1); // file_path
        auto file_path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

        return pxs_newstring(iread_file(file_path.get_string()).c_str());
    }

    // read a directory.
    pxs_VarT read_dir(pxs_VarT args) {
        PXS_ARGC_EQ(1); // dir_path
        auto dir_path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(dir_path.raw(), pxs_String);

        // Get a list of items.
        auto items = iread_dir(dir_path.get_string());

        auto list = pxs::Var::new_list();
        for (const auto& entry : items) {
            list.add(pxs_newstring(entry.c_str()));
        }

        return list.raw();
    }

    // Initialize the `yoyo.fs` module.
    void init(pxs_Module* yoyo) {
        auto _fs = pxs_newmod("fs");
        
        pxs_addfunc(_fs, "read_file", read_file);
        pxs_addfunc(_fs, "read_dir", read_dir);

        pxs_add_submod(yoyo, _fs);
    }
};

#endif // YOYO_FS