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
#include "utils/debug.hpp"
#include <system_error>
#include "types.hpp"
#include "utils/exceptions.hpp"
#include <iterator>

namespace yoyo::fs {
    // free a File
    void free_file(pxs_Opaque obj) {
        if (obj == nullptr) {
            return;
        }

        auto val = static_cast<File*>(obj);
        delete val;
    }

    // Create the `File` object with all it's methods.
    pxs_VarT create_file_object(File* file) {
        auto wrapper = new pxs::type::Wrapper(static_cast<pxs_Opaque>(file), free_file, yoyo::types::FS_FILE_TYPE);
        auto obj = pxs::Object(static_cast<pxs_Opaque>(wrapper), pxs::type::free_wrapper, "File");

        obj.add_method("read", &File::read);
        obj.add_method("write", &File::write);
        obj.add_method("append", &File::append);
        obj.add_method("close", &File::close);
        obj.add_method("remove", &File::remove);
        obj.add_method("save", &File::save);

        return obj.make().raw();
    }

    File::~File() {
        // NO-OP
    }

    File::File(const std::string& path, FileOpenType ot) {
        this->path = path;

        if (ot == FileOpenType::Write) {
            this->ofile = std::make_unique<std::ofstream>(path, std::ios::binary);
        } else if (ot == FileOpenType::Read) {
            this->rfile = std::make_unique<std::ifstream>(path, std::ios::binary);
        } else if (ot == FileOpenType::Append) {
            this->ofile = std::make_unique<std::ofstream>(path, std::ios::binary | std::ios::app);
        }

        this->created = false;
    }

    File::File() {
        this->created = true;
    }

    File* File::self(pxs_VarT arg) {
        auto var = pxs::Var(arg);
        return pxs::type::Wrapper::get<File>(var, yoyo::types::FS_FILE_TYPE);
    }

    pxs_VarT File::create(pxs_VarT args) {
        // create a file.
        auto cfile = new File();
        // wrap and return it.
        return create_file_object(cfile);
    }
    
    pxs_VarT File::open(pxs_VarT args) {
        // Get path and maybe open_type
        auto argc = PXS_ARGC();
        std::string path;
        FileOpenType ot = FileOpenType::Read;
        if (argc >= 1) {
            auto path_arg = pxs::Var::from_args(args, 0);
            PXS_ARG_IS_TYPE(path_arg.raw(), pxs_String);
            path = path_arg.get_string();
            if (argc >= 2) {
                auto ot_arg = pxs::Var::from_args(args, 1);
                PXS_ARG_IS_TYPE(ot_arg.raw(), pxs_Int64);
                ot = static_cast<FileOpenType>(ot_arg.get_uint());
            }
        } else {
            return utils::exceptions::expected_nm(argc, 1);
        }

        // Create file
        auto cfile = new File(path, ot);
        return create_file_object(cfile);
    }

    pxs_VarT File::read(pxs_VarT args) {
        auto self = File::self(PXS_ARG(0));
        if (!self) {
            return utils::exceptions::expected_self(PXS_ARG(0));
        }

        // Assert is read
        if (self->rfile == nullptr) {
            return pxs_newexception("`File` was not opened with `FileOpenType.Read`.");
        }
    
        // Check read type
        FileReadType rt = FileReadType::Text;
        auto rt_arg = pxs::Var::from_args(args, 1);
        if (rt_arg.is(pxs_Int64) || rt_arg.is(pxs_UInt64)) {
            rt = static_cast<FileReadType>(rt_arg.get_uint());
        }

        // Get contents as bytes first.
        std::vector<char> contents(
            (std::istreambuf_iterator<char>(*self->rfile)),
            std::istreambuf_iterator<char>());
        
        if (rt == FileReadType::Text) {
            // convert bytes to text
            std::string text(contents.begin(), contents.end());
            return pxs_newstring(text.c_str());
        } else {
            return yoyo::utils::bytes::make_byte_list(contents);
        }
    }

    pxs_VarT File::write(pxs_VarT args) {
        PXS_ARGC_EQ(2); // self, data
        auto self = File::self(PXS_ARG(0));
        if (!self) {
            return utils::exceptions::expected_self(PXS_ARG(0));
        }

        if (self->ofile == nullptr) {
            return pxs_newexception("`File` was not opened with `FileOpenType.Write or Appen`");
        }

        // CHeck data
        auto data = pxs::Var::from_args(args, 1);
        std::vector<char> bytes;

        if (data.is(pxs_String)) {

        } else if (data.is(pxs_List)) {

        } else {
            
        }
    }

        // // @except
        // // Write into `self`.
        // // args:
        // //  - self: `File`
        // //  - data: `string`|`[]uint` Data to write.
        // static pxs_VarT write(pxs_VarT args);

        // // @except
        // // Append onto `self`.
        // // args:
        // //  - self: `File`
        // //  - data: `string`|`[]uint` Data to append.
        // // 
        // // returns `int` the new size of the file.
        // static pxs_VarT append(pxs_VarT args);

        // // Check if file exists.
        // // args:
        // //  - self: `File`
        // //
        // // returns: `bool`
        // static pxs_VarT exists(pxs_VarT args);

        // // Close a file.
        // // args: 
        // //  - self: `File`
        // static pxs_VarT close(pxs_VarT args);

        // // @except
        // // Remove a file.
        // // args:
        // //  - self: `File`
        // static pxs_VarT remove(pxs_VarT args);

        // // @except
        // // Save a file. This should be used when you created a file vs opened.
        // // args:
        // //  - self: `File`
        // //  - path: `string`
        // static pxs_VarT save(pxs_VarT args);


    // // Internal read file.
    // std::string iread_file(const std::string& path) {
    //     // read file
    //     std::ifstream file(path);
    //     std::vector<std::string> lines;
    //     std::string line;
    //     while (std::getline(file, line)) {
    //         lines.push_back(line);
    //     }
    //     file.close();

    //     return yoyo::utils::str::join(lines);
    // }

    // std::vector<uint8_t> iread_file_bytes(const std::string& path) {
    //     // read file
    //     std::ifstream file(path, std::ios::binary | std::ios::ate);
    //     if (!file.is_open()) {
    //         return {};
    //     }

    //     std::streamsize size = file.tellg();
    //     file.seekg(0, std::ios::beg);
        
    //     std::vector<uint8_t> buffer(size);

    //     if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
    //         return {};
    //     }

    //     return buffer;
    // }

    // // Internal read dir
    // std::vector<std::string> iread_dir(const std::string& path) {
    //     std::filesystem::path dpath(path);
    //     if (!std::filesystem::exists(dpath) || !std::filesystem::is_directory(dpath)) {
    //         return {};
    //     }

    //     std::vector<std::string> list;
    //     // auto list = pxs_newlist();
    //     for (const auto& entry : std::filesystem::directory_iterator(dpath)) {
    //         auto res = entry.path().filename();
    //         list.push_back(res.string());
    //     }

    //     return list;
    // }

    // // Internal write bytes to a file
    // // This could fail if the path is not a file.
    // bool iwrite_file(const std::string& path, const std::vector<unsigned char>& bytes) {
    //     std::ofstream file(path, std::ios::out | std::ios::binary);
    //     if (!file) {
    //         return false;
    //     }

    //     file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    //     file.close();

    //     return true;
    // }

    // // read a file.
    // pxs_VarT read_file(pxs_VarT args) {
    //     PXS_ARGC_EQ(1); // file_path
    //     auto file_path = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

    //     return pxs_newstring(iread_file(file_path.get_string()).c_str());
    // }

    // // read a files bytes.
    // pxs_VarT read_fileb(pxs_VarT args) {
    //     PXS_ARGC_EQ(1); // file_path
    //     auto file_path = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

    //     auto bytes = iread_file_bytes(file_path.get_string());
    //     return yoyo::utils::bytes::make_byte_list(bytes);
    // }

    // // write to a file
    // pxs_VarT write_file(pxs_VarT args) {
    //     PXS_ARGC_EQ(2); // file_path, text
    //     auto file_path = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

    //     auto text = pxs::Var::from_args(args, 1);
    //     if (text.is(pxs_String)) {
    //         // Writing string
    //         auto text_str = text.get_string();
    //         auto res = iwrite_file(file_path.get_string(), std::vector<unsigned char>(text_str.begin(), text_str.end()));
    //         return pxs_newbool(res);
    //     } else if (text.is(pxs_List)) {
    //         // Writing bytes
    //         auto bytes = yoyo::utils::bytes::convert_list_into<unsigned char>(text.raw());
    //         return pxs_newbool(iwrite_file(file_path.get_string(), bytes));
    //     } else {
    //         return pxs_newexception("Expected string or bytes.");
    //     }
    // }

    // // remove a file.
    // pxs_VarT remove_file(pxs_VarT args) {
    //     PXS_ARGC_EQ(1); // file path
    //     auto file_path = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(file_path.raw(), pxs_String);

    //     auto fps = file_path.get_string();

    //     if (!std::filesystem::exists(fps)) {
    //         return pxs_newbool(false);
    //     }
    //     if (std::filesystem::is_directory(fps)) {
    //         return pxs_newexception("Expected file, found directory");
    //     }
    //     return pxs_newbool(std::filesystem::remove(fps));
    // }

    // // remove a empty directory.
    // pxs_VarT remove_empty_dir(pxs_VarT args) {
    //     PXS_ARGC_EQ(1); // dir_path
    //     auto dpath = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(dpath.raw(), pxs_String);

    //     auto dps = dpath.get_string();

    //     if (!std::filesystem::exists(dps)) {
    //         return pxs_newbool(false);
    //     }

    //     if (!std::filesystem::is_empty(dps)) {
    //         return pxs_newbool(false);
    //     }

    //     return pxs_newbool(std::filesystem::remove(dps));
    // }

    // // completely remove a directory
    // pxs_VarT remove_dir(pxs_VarT args) {
    //     PXS_ARGC_EQ(1); // dir_path
    //     auto dpath = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(dpath.raw(), pxs_String);

    //     auto dps = dpath.get_string();

    //     if (!std::filesystem::exists(dps)) {
    //         return pxs_newbool(false);
    //     }

    //     if (!std::filesystem::is_directory(dps)) {
    //         return pxs_newexception("Expected directory, found file");
    //     }
    //     return pxs_newbool(std::filesystem::remove_all(dps) > -1);
    // }

    // // read a directory.
    // pxs_VarT read_dir(pxs_VarT args) {
    //     PXS_ARGC_EQ(1); // dir_path
    //     auto dir_path = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(dir_path.raw(), pxs_String);

    //     // Get a list of items.
    //     auto items = iread_dir(dir_path.get_string());

    //     auto list = pxs::Var::new_list();
    //     for (const auto& entry : items) {
    //         list.add(pxs_newstring(entry.c_str()));
    //     }

    //     return list.raw();
    // }

    // // Create a directory
    // pxs_VarT create_dir(pxs_VarT args) {
    //     PXS_ARGC_EQ(1); // Dir path
    //     auto dir_path = pxs::Var::from_args(args, 0);
    //     PXS_ARG_IS_TYPE(dir_path.raw(), pxs_String);

    //     std::error_code ec;

    //     std::filesystem::create_directories(dir_path.get_string(), ec);

    //     if (ec) {
    //         return pxs_newexception(ec.message().c_str());
    //     }

    //     return pxs_newnull();
    // }

    // Initialize the `yoyo.fs` module.
    void init(pxs_Module* yoyo) {
        auto _fs = pxs_newmod("fs");

        pxs_addfunc(_fs, "read_file", read_file);
        pxs_addfunc(_fs, "write_file", )
        
        // pxs_addfunc(_fs, "read_file", read_file);
        // pxs_addfunc(_fs, "read_fileb", read_fileb);
        // pxs_addfunc(_fs, "read_dir", read_dir);
        // pxs_addfunc(_fs, "write_file", write_file);
        // pxs_addfunc(_fs, "remove_file", remove_file);
        // pxs_addfunc(_fs, "remove_empty_dir", remove_empty_dir);
        // pxs_addfunc(_fs, "remove_dir", remove_dir);
        // pxs_addfunc(_fs, "create_dir", create_dir);

        pxs_add_submod(yoyo, _fs);
    }
};

#endif // YOYO_FS