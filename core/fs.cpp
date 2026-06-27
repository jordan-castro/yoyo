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
        if (this->created) {
            return;
        }

        if (this->stream.is_open()) {
            this->stream.close();
        }
    }

    File::File(const std::string& path, FileOpenType ot) {
        this->path = path;

        this->opentype = ot;

        std::ios_base::openmode mode = 0;

        if (ot & FileOpenType::Read) {
            mode |= std::ios::in;
        }
        if (ot & FileOpenType::Write) {
            mode |= std::ios::out;
        }
        if (ot & FileOpenType::Append) {
            mode |= std::ios::app;
        }

        this->stream = std::fstream(path, mode);
        this->created = false;
    }

    File::File() {
        this->created = true;
        FileOpenType ot = FileOpenType::Read | FileOpenType::Write | FileOpenType::Append;
        this->opentype = ot;
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

        // If the path does not exist, create it
        if (!std::filesystem::exists(path)) {
            // Create the file
            std::ofstream blank(path);
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
        if (!(self->opentype & FileOpenType::Read)) {
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
            (std::istreambuf_iterator<char>(self->stream)),
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

        if (!(self->opentype & FileOpenType::Write)) {
            return pxs_newexception("`File` was not opened with `FileOpenType.Write`");
        }

        // CHeck data
        auto data = pxs::Var::from_args(args, 1);
        std::vector<char> bytes;

        if (data.is(pxs_String)) {
            auto str = data.get_string();
            bytes = std::vector<char>(bytes.begin(), bytes.end());
        } else if (data.is(pxs_List)) {
            bytes = utils::bytes::convert_list_into<char>(data.raw());
        } else {
            return utils::exceptions::expected_types(data.raw()->tag, {pxs_String, pxs_List});
        }

        self->stream.write(reinterpret_cast<const char*>(bytes.data(), bytes.size()), bytes.size());

        return pxs_newnull();
    }

    pxs_VarT File::append(pxs_VarT args) {
        PXS_ARGC_EQ(2); // self, data
        auto self = File::self(PXS_ARG(0));
        if (!self) {
            return utils::exceptions::expected_self(PXS_ARG(0));
        }

        PXS_ARGC_EQ(2); // self, data
        auto self = File::self(PXS_ARG(0));
        if (!self) {
            return utils::exceptions::expected_self(PXS_ARG(0));
        }

        if (!(self->opentype & FileOpenType::Append)) {
            return pxs_newexception("`File` was not opened with `FileOpenType.Append`");
        }

        // CHeck data
        auto data = pxs::Var::from_args(args, 1);
        std::vector<char> bytes;

        if (data.is(pxs_String)) {
            auto str = data.get_string();
            bytes = std::vector<char>(bytes.begin(), bytes.end());
        } else if (data.is(pxs_List)) {
            bytes = utils::bytes::convert_list_into<char>(data.raw());
        } else {
            return utils::exceptions::expected_types(data.raw()->tag, {pxs_String, pxs_List});
        }

        self->stream.write(reinterpret_cast<const char*>(bytes.data(), bytes.size()), bytes.size());

        return pxs_newnull();
    }

    pxs_VarT File::close(pxs_VarT args) {
        auto self = File::self(PXS_ARG(0));
        if (!self) {
            return utils::exceptions::expected_self(PXS_ARG(0));
        }

        // Check if created
        if (self->created) {
            return pxs_newnull();
        }

        if (self->stream.is_open()) {
            // Actually close the file.
            self->stream.close();
        }
        return pxs_newnull();
    }

    pxs_VarT File::remove(pxs_VarT args) {
        auto self = File::self(PXS_ARG(0));
        if (!self) {
            return utils::exceptions::expected_self(PXS_ARG(0));
        }

        if (self->created) {
            // reset buffer
            self->buffer.clear();
        } else {
            // remove file.
            std::error_code ec;
            std::filesystem::remove(self->path, ec);

            if (ec) {
                return pxs_newexception(ec.message().c_str());
            }
        }
        return pxs_newnull();
    }

    pxs_VarT File::save(pxs_VarT args) {
        PXS_ARGC_EQ(2); // self, path to save to.
        auto self = File::self(PXS_ARG(0));
        if (!self) {
            return utils::exceptions::expected_self(PXS_ARG(0));
        }

        auto path_var = pxs::Var::from_args(args, 1);
        PXS_ARG_IS_TYPE(path_var.raw(), pxs_String);

        if (self->created) {
            // Creates it automatically yo!
            std::ofstream w(path_var.get_string(), std::ios::out);
            w << self->buffer.rdbuf();
            w.close();
        }

        return pxs_newnull();
    }

    pxs_VarT read_file(pxs_VarT args) {
        PXS_ARGC_GT(0); // at least path.
        auto path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(path.raw(), pxs_String);
        auto read_type = pxs::Var::from_args(args, 1);
        if (!read_type.is(pxs_Int64) && !read_type.is(pxs_UInt64)) {
            return utils::exceptions::expected_types(read_type.raw()->tag, {pxs_Int64, pxs_UInt64});            
        }

        // open file
        auto file = pxs::Var(pxs::call(&File::open, {path.shallow().raw()}));
        if (!file.is(pxs_HostObject)) {
            // Exception?
            return file.raw();
        }

        // This will automatically drop file.
        return pxs::call(&File::read, {file.raw(), path.shallow().raw(), read_type.shallow().raw()});
    }

    pxs_VarT read_dir(pxs_VarT args) {
        auto path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(path.raw(), pxs_String);
        auto path_str = path.get_string();
        std::filesystem::path dpath(path_str);
        if (!std::filesystem::exists(dpath)) {
            return pxs_newexception("Directory does not exist.");
        }
        if (std::filesystem::is_directory(dpath)) {
            return pxs_newexception("Path is not a directory.");
        }
        auto list = pxs::Var::new_list();
        for (const auto& entry : std::filesystem::directory_iterator(dpath)) {
            auto res = entry.path().filename();
            list.add(pxs::from_variant(res.string()));
        }
        return list.raw();
    }

    pxs_VarT write_file(pxs_VarT args) {
        PXS_ARGC_EQ(2); // path, data
        auto path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(path.raw(), pxs_String);

        auto data = pxs::Var::from_args(args, 1);
        if (!data.is(pxs_String) && !data.is(pxs_List)) {
            return utils::exceptions::expected_types(data.raw()->tag, {pxs_String, pxs_List});
        }

        // Open the file
        auto file = pxs::Var(pxs::call(&File::open, {path.shallow().raw()}));
        if (!file.is(pxs_HostObject)) {
            return file.raw();
        }

        return pxs::call(&File::write, {file.raw(), data.shallow().raw()});
    }

    pxs_VarT exists(pxs_VarT args) {
        PXS_ARGC_EQ(1); // path
        auto path = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(path.raw(), pxs_String);
        auto str = path.get_string();
        return pxs_newbool(std::filesystem::exists(str));
    }

    pxs_VarT remove_dir(pxs_VarT args) {
        PXS_ARGC_GT(0); // path, rt
        // PXS_ARG_STRING_VAL(path, 0);
        // PXS_ARG_NUMBER_Val(rt, 1);
    }

    // // @except
    // // Remove a directory.
    // // args:
    // //  - path: `string` path to the directory.
    // //  - rt: `DirRemoveType` to remove empty or all. Default empty
    // pxs_VarT remove_dir(pxs_VarT args);

    // // @except
    // // Remove a file.
    // // args:
    // //  - path: `string` path to the file.
    // pxs_VarT remove_file(pxs_VarT args);

    // // @except
    // // Create a directory.
    // // args:
    // //  - path: `string` directory path
    // //  - mode: `CreateDirMode` defaults to `Single`.
    // pxs_VarT create_dir(pxs_VarT args);

    // Check if path is a directory.
    // args:
    //  - path: `string` path
    //
    // returns `bool`
    // pxs_VarT is_dir(pxs_VarT args);

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
        pxs_addfunc(_fs, "write_file", write_file);
        pxs_addfunc(_fs, "read_dir", read_dir);
        pxs_addfunc(_fs, "exists", exists);
        pxs_addfunc(_fs, "remove_dir", remove_dir);
        pxs_addfunc(_fs, "remove_file", remove_file);
        pxs_addfunc(_fs, "create_dir", create_dir);
        pxs_addfunc(_fs, "create", &File::create);
        pxs_addfunc(_fs, "open", &File::open);

        pxs_add_submod(yoyo, _fs);
    }
};

#endif // YOYO_FS