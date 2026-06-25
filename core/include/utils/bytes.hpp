#pragma once

#include <pixelscript.h>
#include <vector>
#include <cstdint>

namespace yoyo::utils::bytes {
    // Utility for creating a bytes list.
    template<typename T>
    pxs_VarT make_byte_list(const std::vector<T>& bytes) {
        auto byte_list = pxs_newlist();
        for (size_t i = 0; i < bytes.size(); i++) {
            pxs_listadd(byte_list, pxs_newuint(static_cast<uint8_t>(bytes[i])));
        }
        return byte_list;
    }
    // Utility for creating a bytes list from pointer, size.
    template<typename T>
    pxs_VarT make_byte_list(T* data, size_t size) {
        auto byte_list = pxs_newlist();
        for (size_t i = 0; i < size; i++) {
            pxs_listadd(byte_list, pxs_newuint(static_cast<uint8_t>(data[i])));
        }
        return byte_list;
    }
    // Convert a `pxs_List` into a std::vector<T> of bytes.
    template<typename T>
    std::vector<T> convert_list_into(pxs_VarT list) {
        auto len = pxs_listlen(list);
        std::vector<T> bytes(len);

        for (int i = 0; i < len; i++) {
            bytes.push_back(static_cast<T>(pxs_getuint(pxs_listget(list, i))));
        }
        return bytes;
    }
}