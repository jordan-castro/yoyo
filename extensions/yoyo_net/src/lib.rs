use etffi::{borrow_string, ptr_magic::PtrMagic};

mod http;
mod utils;

use http::client::ClientResponse;

/// http::client. Make a get response from just URL.
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_get(url: *const core::ffi::c_char) -> *mut ClientResponse {
    if url.is_null() {
        return core::ptr::null_mut();
    }

    let response = http::client::get(borrow_string!(url).to_string());
    match response {
        Ok(r) => r.into_raw(),
        Err(_) => core::ptr::null_mut()
    }
}

/// http::client. free a ClientResponse
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_free(ptr: *mut ClientResponse) {
    if ptr.is_null() {
        return;
    }

    let _ = ClientResponse::from_raw(ptr as *mut ClientResponse);
}

/// http::client. get response bytes
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_getbytes(ptr: *mut ClientResponse) ->  {
    if ptr.is_null() {
        return;
    }


}