#ifdef YOYO_FS

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "fs.hpp"
#include "utils/bytes.hpp"
#include "utils/strutils.hpp"

namespace yoyo::fs {
    // Internal read file.
    std::string iread_file(const std::string& path) {
        // read file
        std::ifstream file(path);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();

        return yoyo::utils::str::join(lines);
    }

    std::vector<uint8_t> iread_file_bytes(const std::string& path) {
        // read file
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return {};
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> buffer(size);

        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            return {};
        }

        return buffer;
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

    // Internal write bytes to a file
    bool iwrite_file(const std::string& path, const std::vector<unsigned char>& bytes) {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        if (!file) {
            return false;
        }

        file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        file.close();

        return true;
    }

    // read a file.
    pxs_VarT read_file(pxs_VarT args) {
        PXS_ARGC_EQ(1); // file_path
        auto file_path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

        return pxs_newstring(iread_file(file_path.get_string()).c_str());
    }

    // read a files bytes.
    pxs_VarT read_fileb(pxs_VarT args) {
        PXS_ARGC_EQ(1); // file_path
        auto file_path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

        auto bytes = iread_file_bytes(file_path.get_string());
        return yoyo::utils::bytes::make_byte_list(bytes);
    }

    // write to a file
    pxs_VarT write_file(pxs_VarT args) {
        PXS_ARGC_EQ(2); // file_path, text
        auto file_path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

        auto text = pxs::Var::from_args(args, 1);
        if (text.is(pxs_String)) {
            // Writing string
            auto text_str = text.get_string();
            auto res = iwrite_file(file_path.get_string(), std::vector<unsigned char>(text_str.begin(), text_str.end()));
            return pxs_newbool(res);
        } else if (text.is(pxs_List)) {
            // Writing bytes
            auto bytes = yoyo::utils::bytes::convert_list_into<unsigned char>(text.raw());
            return pxs_newbool(iwrite_file(file_path.get_string(), bytes));
        } else {
            return pxs_newexception("Expected string or bytes.");
        }
    }

    // remove a file.
    pxs_VarT remove_file(pxs_VarT args) {
        PXS_ARGC_EQ(1); // file path
        auto file_path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

        auto fps = file_path.get_string();

        if (!std::filesystem::exists(fps)) {
            return pxs_newbool(false);
        }
        if (std::filesystem::is_directory(fps)) {
            return pxs_newexception("Expected file, found directory");
        }
        return pxs_newbool(std::filesystem::remove(fps));
    }

    // remove a empty directory.
    pxs_VarT remove_empty_dir(pxs_VarT args) {
        PXS_ARGC_EQ(1); // dir_path
        auto dpath = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(dpath.raw(), pxs_String);

        auto dps = dpath.get_string();

        if (!std::filesystem::exists(dps)) {
            return pxs_newbool(false);
        }

        if (!std::filesystem::is_empty(dps)) {
            return pxs_newbool(false);
        }

        return pxs_newbool(std::filesystem::remove(dps));
    }

    // completely remove a directory
    pxs_VarT remove_dir(pxs_VarT args) {
        PXS_ARGC_EQ(1); // dir_path
        auto dpath = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(dpath.raw(), pxs_String);

        auto dps = dpath.get_string();

        if (!std::filesystem::exists(dps)) {
            return pxs_newbool(false);
        }

        if (!std::filesystem::is_directory(dps)) {
            return pxs_newexception("Expected directory, found file");
        }
        return pxs_newbool(std::filesystem::remove_all(dps) > -1);
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
        pxs_addfunc(_fs, "read_fileb", read_fileb);
        pxs_addfunc(_fs, "read_dir", read_dir);
        pxs_addfunc(_fs, "write_file", write_file);
        pxs_addfunc(_fs, "remove_file", remove_file);
        pxs_addfunc(_fs, "remove_empty_dir", remove_empty_dir);
        pxs_addfunc(_fs, "remove_dir", remove_dir);

        pxs_add_submod(yoyo, _fs);
    }
};

#endif // YOYO_FS