#pragma once

#include <pixelscript.h>
#include <pixelscript_cpp.hpp>

#include <cstdint>
#include <string>

namespace yoyo {
    class YoyoType {
        // Internal structure.
        pxs_Opaque data;
        // Function for freeing memory.
        pxs_DeleterFn deleter;
        // To know what type your object is.
        // -1 invalid or unset and could cause UB.
        // Setting different types the same tag is UB.
        int32_t type_tag = -1;

    public:
        YoyoType(pxs_Opaque data, pxs_DeleterFn deleter, int32_t tt) : data(data), deleter(deleter), type_tag(tt) {}
        ~YoyoType() {
            if (this->data != nullptr) {
                this->deleter(this->data);
                this->data = nullptr;
                this->type_tag = -1;
            }
        }

        // Get the data.
        template<typename T>
        static T* get(const pxs::Var& var, int32_t expected_type) {
            auto wrapper = var.get_object<YoyoType>();
            if (!wrapper) {
                return nullptr;
            }

            // Check type matches
            if (wrapper->type_tag != expected_type) {
                return nullptr;
            }

            return static_cast<T*>(wrapper->data);
        }
    };

    // Initialize the yoyo module.
    void init(int argc, char* argv[]);
    // Stop the yoyo runtime.
    void stop();
    // Run source code
    pxs::Var run(pxs_Runtime runtime, const std::string& code, const std::string& file_name);
};
