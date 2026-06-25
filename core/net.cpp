#ifdef YOYO_NET

#include "net.hpp"
#include <pixelscript_cpp.hpp>
#include "types.hpp"
#include <yoyo_net.h>
#include "utils/debug.hpp"
#include "utils/bytes.hpp"

namespace yoyo::net {
    // Helpful function to get `self`
    yoyonet_ClientResponse* _get_response(const pxs::Var& var) {
        return pxs::type::Wrapper::get<yoyonet_ClientResponse>(var, yoyo::types::NET_HTTP_RESPONSE_TYPE);
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

        return yoyo::utils::bytes::make_byte_list(bytes->bytes, bytes->size);
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
        auto wrapper = new pxs::type::Wrapper(static_cast<pxs_Opaque>(response), free_http_response, yoyo::types::NET_HTTP_RESPONSE_TYPE);
        auto obj = pxs::Object(static_cast<pxs_Opaque>(wrapper), pxs::type::free_wrapper, "ClientResponse");

        // Methods go here.
        obj.add_property("version", CR_version);
        obj.add_property("status", CR_status);
        obj.add_property("text", CR_text);
        obj.add_property("bytes", CR_bytes);

        return obj.make().raw();
    }

    // Make a GET request
    pxs_VarT get(pxs_VarT args) {
        PXS_ARGC_GT(1); // URL:string, headers:list[list[String, String]], version:int
        auto url_arg = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(url_arg.raw(), pxs_String);
        auto url = url_arg.get_string();

        // TODO: Implement all the different fields on options.
        auto options = yoyonet_httpclient_options_new();
        yoyonet_httpclient_options_seturl(options, url.c_str());

        auto response = yoyonet_httpclient_request(options);
        if (response == nullptr) {
            return pxs_newexception("Could not GET");
        }

        // Free options
        yoyonet_httpclient_options_free(options);

        return _setup_response(response);
    }

    void init(pxs_Module* yoyo_mod) {
        auto net_mod = pxs_newmod("net");

        pxs_addfunc(net_mod, "get", get);
        pxs_addvar(net_mod, "HTTP_VERSION_09", pxs_newint(YOYONET_HTTP_VERSION_09));
        pxs_addvar(net_mod, "HTTP_VERSION_10", pxs_newint(YOYONET_HTTP_VERSION_10));
        pxs_addvar(net_mod, "HTTP_VERSION_11", pxs_newint(YOYONET_HTTP_VERSION_11));
        pxs_addvar(net_mod, "HTTP_VERSION_2", pxs_newint(YOYONET_HTTP_VERSION_2));
        pxs_addvar(net_mod, "HTTP_VERSION_3", pxs_newint(YOYONET_HTTP_VERSION_3));

        pxs_add_submod(yoyo_mod, net_mod);
    }
};

#endif // YOYO_NET

