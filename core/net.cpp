#ifdef YOYO_NET
#include "net.hpp"
#include <pixelscript_cpp.hpp>
#include "types.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "lib/httplib.h"

// struct Response {
//   std::string version;
//   int status = -1;
//   std::string reason;
//   Headers headers;
//   Headers trailers;
//   std::string body;
//   std::string location; // Redirect location

//   // User-defined context — set by pre-routing/pre-request handlers and read
//   // by route handlers to pass arbitrary data (e.g. decoded auth tokens).
//   UserData user_data;

//   bool has_header(const std::string &key) const;
//   std::string get_header_value(const std::string &key, const char *def = "",
//                                size_t id = 0) const;
//   size_t get_header_value_u64(const std::string &key, size_t def = 0,
//                               size_t id = 0) const;
//   size_t get_header_value_count(const std::string &key) const;
//   void set_header(const std::string &key, const std::string &val);

//   bool has_trailer(const std::string &key) const;
//   std::string get_trailer_value(const std::string &key, size_t id = 0) const;
//   size_t get_trailer_value_count(const std::string &key) const;

//   void set_redirect(const std::string &url, int status = StatusCode::Found_302);
//   void set_content(const char *s, size_t n, const std::string &content_type);
//   void set_content(const std::string &s, const std::string &content_type);
//   void set_content(std::string &&s, const std::string &content_type);

//   void set_content_provider(
//       size_t length, const std::string &content_type, ContentProvider provider,
//       ContentProviderResourceReleaser resource_releaser = nullptr);

//   void set_content_provider(
//       const std::string &content_type, ContentProviderWithoutLength provider,
//       ContentProviderResourceReleaser resource_releaser = nullptr);

//   void set_chunked_content_provider(
//       const std::string &content_type, ContentProviderWithoutLength provider,
//       ContentProviderResourceReleaser resource_releaser = nullptr);

//   void set_file_content(const std::string &path,
//                         const std::string &content_type);
//   void set_file_content(const std::string &path);

//   Response() = default;
//   Response(const Response &) = default;
//   Response &operator=(const Response &) = default;
//   Response(Response &&) = default;
//   Response &operator=(Response &&) = default;
//   ~Response() {
//     if (content_provider_resource_releaser_) {
//       content_provider_resource_releaser_(content_provider_success_);
//     }
//   }

//   // private members...
//   size_t content_length_ = 0;
//   ContentProvider content_provider_;
//   ContentProviderResourceReleaser content_provider_resource_releaser_;
//   bool is_chunked_content_provider_ = false;
//   bool content_provider_success_ = false;
//   std::string file_content_path_;
//   std::string file_content_content_type_;
// };


namespace yoyo::net {
    void free_http_response(pxs_Opaque obj) {
        if (!obj) {
            return;
        }

        auto val = static_cast<httplib::Response*>(obj);
        delete val;
    }

    // ClientResponse.version
    pxs_VarT clientresponse_version_prop(pxs_VarT args) {
        auto self = pxs::type::Wrapper::get<httplib::Response>(pxs::Var::from_args(args, 0), yoyo::types::HTTP_RESPONSE_TYPE);
        if (self == nullptr) {
            return pxs_newexception("`self` is required.");
        }

        return pxs_newstring(self->version.c_str());
    }
    
    // ClientResponse.status
    pxs_VarT clientresponse_status_prop(pxs_VarT args) {
        auto self = pxs::type::Wrapper::get<httplib::Response>(pxs::Var::from_args(args, 0), yoyo::types::HTTP_RESPONSE_TYPE);
        if (self == nullptr) {
            return pxs_newexception("`self` is required.");
        }
        return pxs_newint(static_cast<int>(self->status));
    }

    // ClientResponse.body
    pxs_VarT clientresponse_body_prop(pxs_VarT args) {
        auto self = pxs::type::Wrapper::get<httplib::Response>(pxs::Var::from_args(args, 0), yoyo::types::HTTP_RESPONSE_TYPE);
        if (self == nullptr) {
            return pxs_newexception("`self` is required.");
        }
        return pxs_newstring(self->body.c_str());
    }

    // Setup the response. This is useful because multiple mediums can create a ClientResponse.
    pxs_VarT setup_response(httplib::Response* response) {
        auto wrapper = new pxs::type::Wrapper(static_cast<pxs_Opaque>(response), free_http_response, yoyo::types::HTTP_RESPONSE_TYPE);
        auto obj = pxs::Object(static_cast<pxs_Opaque>(wrapper), pxs::type::free_wrapper, "ClientResponse");

        // Methods go here.
        obj.add_property("version", clientresponse_version_prop);
        obj.add_property("status", clientresponse_status_prop);
        obj.add_property("body", clientresponse_body_prop);

        return obj.make().raw();
    }

    // Make a GET request
    pxs_VarT get(pxs_VarT args) {
        PXS_ARGC_EQ(1); // URL.
        auto url_arg = pxs::Var::from_args(args, 0);
        PXS_ARG_IS_TYPE(url_arg.raw(), pxs_String);
        auto url = url_arg.get_string();

        httplib::Client cli(url);

        if (auto res = cli.Get("")) {
            return setup_response(new httplib::Response(*res));
        } else {
            auto err = res.error();
            return pxs_newexception(httplib::to_string(err).c_str());
        }
    }

    void init(pxs_Module* yoyo_mod) {
        auto net_mod = pxs_newmod("net");

        pxs_addfunc(net_mod, "get", get);

        pxs_add_submod(yoyo_mod, net_mod);
    }
};

#endif // YOYO_NET