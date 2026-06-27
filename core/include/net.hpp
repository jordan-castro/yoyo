#pragma once

#ifdef YOYO_NET
#include <pixelscript_cpp.hpp>
#include <yoyo_net.h>

namespace yoyo::net {
    // A class that creates `ClientResponse`.
    class Client : public pxs::type::Extension<Client> {
        // @private
        // This is all the `Client` is. A holder of `yoyonet_ClientOptions`.
        yoyonet_ClientOptions* co;
    public:
        
    };

    // A wrapper around `yoyonet_ClientResponse`.
    // Is PXS native.
    class ClientResponse : public pxs::type::Extension<ClientResponse> {
        // @private
        // The internal client response from `yoyo_net` crate.
        yoyonet_ClientResponse* cr;
    public:
        // @prop
        // Get the HTTP version.
        // args:
        //  - self: `ClientResponse`
        // 
        // returns `int`
        static pxs_VarT get_version(pxs_VarT args);
        
        // @prop
        // Get the response status.
        // args:
        //  - self: `ClientResponse`
        //
        // returns `int`
        static pxs_VarT get_status(pxs_VarT args);

        // @prop
        // Get the response bytes.
        // args:
        //  - self: `ClientResponse`
        //
        // returns `[]uint`
        static pxs_VarT get_bytes(pxs_VarT args);

        // @prop
        // Get the response text.
        // args:
        //  - self: `ClientResponse`
        // 
        // returns `string`
        static pxs_VarT get_text(pxs_VarT args);
    };

    // Make a HTTP Get request.
    // args:
    //  - url: `string` the url to request to.
    //  - headers: @opt `[][]string` the headers to apply.
    //  - version: @opt `int` the HTTP version to use.
    //
    // returns `ClientResponse`
    pxs_VarT get(pxs_VarT args);

    void init(pxs_Module* yoyo_mod);
};

#endif // YOYO_NET