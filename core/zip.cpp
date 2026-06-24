#ifdef YOYO_ZIP

#ifdef YOYO_FS
#include "fs.hpp"
#endif // YOYO_FS

#include "zip.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include "miniz.hpp"

namespace yoyo::zip {
    // The structure for working with zip files.
    struct ZipFile {
        miniz_cpp::zip_file file;
    };

    // Free a ZipFile
    void free_zip_file(pxs_Opaque obj) {
        if (obj == nullptr) {
            return;
        }

        auto val = static_cast<ZipFile*>(obj);
        val->file.printdir();
        delete val;
    }

    // Read a file or stream directly.
    pxs_VarT read(pxs_VarT args) {
        PXS_ARGC_EQ(1); // File path.
        auto file_path_var = pxs::Var::from_args(args, 0);
        std::vector<uint8_t> bytes;
        if (file_path_var.is(pxs_String)) {
            #ifdef YOYO_FS
            // Read the file and stream it
            bytes = yoyo::fs::iread_file_bytes(file_path_var.get_string());
            #else
                return pxs_newexception("`zip.read` expects bytes not string.");
            #endif // YOYO_FS
        } else if (file_path_var.is(pxs_List)) {
            // Get bytes
            std::vector<uint8_t> bytes;
            for (int i = 0; i < file_path_var.list_len(); i++) {
                auto byte = file_path_var.list_get(i).get_uint();
                bytes.push_back(byte);
            }
        }

        
    }

    void init(pxs_Module* yoyo) {
        auto zip_mod = pxs_newmod("zip");

        pxs_add_submod(yoyo, zip_mod);
    }
};

#endif // YOYO_ZIP