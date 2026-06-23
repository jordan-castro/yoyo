#ifdef YOYO_NET

#include "net.hpp"
#include <pixelscript_cpp.hpp>
#include "types.hpp"
#include <yoyo_net.h>

namespace yoyo::net {
    // Helpful function to get `self`
    yoyonet_ClientResponse* _get_response(const pxs::Var& var) {
        return pxs::type::Wrapper::get<yoyonet_ClientResponse>(var, yoyo::types::HTTP_RESPONSE_TYPE);
    }

    // Free a ClientResponse
    void free_http_response(pxs_Opaque obj) {
        if (obj == nullptr) {
            return;
        }

        auto val = static_cast<yoyonet_ClientResponse*>(obj);
        yoyonet_httpclient_free(val);
    }

    // ClientResponse.version
    pxs_VarT CR_version(pxs_VarT args) {
        auto self = _get_response(pxs::Var::from_args(args, 0));
        if (self == nullptr) {
            return pxs_newexception("`self` is required.");
        }

        return pxs_newint(self->version);
    }

    // ClientResponse.status
    pxs_VarT CR_status(pxs_VarT args) {
        auto self = _get_response(pxs::Var::from_args(args, 0));
        if (self == nullptr) {
            return pxs_newexception("`self` is required.");
        }

        return pxs_newint(self->status);
    }

    // ClientResponse.bytes
    pxs_VarT CR_bytes(pxs_VarT args) {
        auto self = _get_response(pxs::Var::from_args(args, 0));
        if (self == nullptr) {
            return pxs_newexception("`self` is required.");
        }

        auto bytes_list = pxs_newlist();
        auto bytes = self->bytes;
        if (bytes == nullptr) {
            return pxs_newnull();
        }

        if (bytes->bytes == nullptr) {
            return bytes_list;
        }

        // Populate list
        for (int i = 0; i < bytes->size; i++) {
            auto byte = bytes->bytes[i];
            pxs_listadd(bytes_list, pxs_newuint(byte));
        }

        return bytes_list;
    }

    // ClientResponse.text
    pxs_VarT CR_text(pxs_VarT args) {
        auto self = _get_response(pxs::Var::from_args(args, 0));
        if (self == nullptr) {
            return pxs_newexception("`self` is required.");
        }

        auto bytes = self->bytes;
        if (bytes == nullptr || bytes->bytes == nullptr) {
            return pxs_newnull();
        }

        std::string text = std::string(reinterpret_cast<const char*>(bytes->bytes), bytes->size);
        return pxs_newstring(text.c_str());
    }

    // Setup the response. This is useful because multiple mediums can create a ClientResponse.
    pxs_VarT _setup_response(yoyonet_ClientResponse* response) {
        auto wrapper = new pxs::type::Wrapper(static_cast<pxs_Opaque>(response), free_http_response, yoyo::types::HTTP_RESPONSE_TYPE);
        auto obj = pxs::Object(static_cast<pxs_Opaque>(wrapper), pxs::type::free_wrapper, "ClientResponse");

        // Methods go here.
        obj.add_property("version", CR_version);
        obj.add_property("status", CR_status);
        obj.add_property("text", CR_text);

        return obj.make().raw();
    }

    // Make a GET request
    pxs_VarT get(pxs_VarT args) {
        PXS_ARGC_EQ(1); // URL.
        auto url_arg = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(url_arg.raw(), pxs_String);
        auto url = url_arg.get_string();

        auto response = yoyonet_httpclient_get(url.c_str());
        if (response == nullptr) {
            return pxs_newexception("Could not GET");
        }

        return _setup_response(response);
    }

    void init(pxs_Module* yoyo_mod) {
        auto net_mod = pxs_newmod("net");

        pxs_addfunc(net_mod, "get", get);

        pxs_add_submod(yoyo_mod, net_mod);
    }
};

#endif // YOYO_NET

