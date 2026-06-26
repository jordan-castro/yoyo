#pragma once

#ifdef YOYO_FS

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <fstream>
#include <sstream>

namespace yoyo::fs {
    // Pass this as the final argument in `read_file` to return bytes or text.
    // Defaults to Text
    enum class FileReadType : uint8_t {
        Bytes,
        Text
    };

    // Pass this as the final argument in `open_file` to open as Read, Write, or Append
    // Defaults to Read
    enum class FileOpenType : uint8_t {
        Read,
        Write,
        Append
    };

    // Pass this as the final argument in `remove_dir` to either remove only empty components or all components.
    // Defaults to Empty.
    enum class DirRemoveType : uint8_t {
        Empty,
        All
    };

    // Wraps a `ifstream` or `ofstream` depending on `FileOpenType`.
    // Use this when memory needs to be explicit.
    // `read_file` and `write_file` use `File` internally.
    class File {
        // @private
        // File path
        std::string path;
        // @private
        // Potentially the stream for reading.
        std::unique_ptr<std::ifstream> rfile;
        // @private
        // Potentially the stream for writing.
        std::unique_ptr<std::ofstream> ofile;
        // @private
        // a buffer for writing, when the file is created not opened.
        std::stringstream buffer;

        // @private
        // Was this file created or opened?
        bool created;

        // When opening a file.
        File(const std::string& fpath, FileOpenType ot);
        // When creating a file.
        File();
    
    public:
        ~File();
        // @private
        // 
        // Get `self`.
        static File* self(pxs_VarT arg);

        // Create a new `File`. It won't write to the system until `save` is called.
        //
        // returns `File` a new instance.
        static pxs_VarT create(pxs_VarT args);

        // @except
        // 
        // Open a `File`. If it does not exist, it creates it on the fly, but `save` is not required.
        // args:
        //  - path: `string` path to the file.
        //  - open_type: `FileOpenType` how to open the file. Defaults to `FileOpenType::Read`.
        //
        // returns `File` a new instance.
        static pxs_VarT open(pxs_VarT args);

        // @except
        // Read from `self`.
        // args:
        //  - self: `File`
        //  - read_type: `FileReadType` what to return the file contents as. Defaults to `FileReadType::Text`.
        //
        // returns `string`|`[]uint` Text or Bytes depending on `read_type`.
        static pxs_VarT read(pxs_VarT args);

        // @except
        // Write into `self`.
        // args:
        //  - self: `File`
        //  - data: `string`|`[]uint` Data to write.
        static pxs_VarT write(pxs_VarT args);

        // @except
        // Append onto `self`.
        // args:
        //  - self: `File`
        //  - data: `string`|`[]uint` Data to append.
        // 
        // returns `int` the new size of the file.
        static pxs_VarT append(pxs_VarT args);

        // Close a file.
        // args: 
        //  - self: `File`
        static pxs_VarT close(pxs_VarT args);

        // @except
        // Remove a file.
        // args:
        //  - self: `File`
        static pxs_VarT remove(pxs_VarT args);

        // @except
        // Save a file. This should be used when you created a file vs opened.
        // args:
        //  - self: `File`
        //  - path: `string`
        static pxs_VarT save(pxs_VarT args);
    };

    // Read a file.
    // args:
    //  - path: `string` path to the file.
    //  - read_type: `FileReadType` how to read the file. Defaults to `FileReadType::Text`.
    // 
    // returns `string`|`[]uint`
    pxs_VarT read_file(pxs_VarT args);

    // Read the entries of a directory.
    // args:
    //  - path: `string` path to the directory.
    //
    // returns `[]string` file entries.
    pxs_VarT read_dir(pxs_VarT args);

    // @except
    // Write to a file.
    // args:
    //  - path: `string` path to the file.
    //  - data: `string`|`[]uint` the data to write.
    pxs_VarT write_file(pxs_VarT args);

    // Check if a file/dir exists.
    // args:
    //  - path: `string` file/dir path.
    //
    // returns: `bool`
    pxs_VarT exists(pxs_VarT args);

    // @except
    // Remove a directory.
    // args:
    //  - path: `string` path to the directory.
    //  - rt: `DirRemoveType` to remove empty or all. Default empty
    pxs_VarT remove_dir(pxs_VarT args);

    // @except
    // Remove a file.
    // args:
    //  - path: `string` path to the file.
    pxs_VarT remove_file(pxs_VarT);

    // @private
    // 
    // Initialize the `yoyo.fs` module.
    void init(pxs_Module* yoyo);
};

#endif // YOYO_FS