use etffi::{borrow_string, ptr_magic::PtrMagic};
use http::client::yoyonet_ClientResponse;

mod http;
mod utils;

/// http::client. Make a get response from just URL.
/// 
/// url: BORROW
/// return: OWNED
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_get(url: *const core::ffi::c_char) -> *mut yoyonet_ClientResponse {
    if url.is_null() {
        return core::ptr::null_mut();
    }

    let response = http::client::get(borrow_string!(url).to_string());
    match response {
        Ok(r) => r.into_raw(),
        Err(_) => core::ptr::null_mut()
    }
}

/// http::client. free a ClientResponse.
/// 
/// ptr: TRANSFER
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_free(ptr: *mut yoyonet_ClientResponse) {
    if ptr.is_null() {
        return;
    }

    let _ = yoyonet_ClientResponse::from_raw(ptr as *mut yoyonet_ClientResponse);
}
