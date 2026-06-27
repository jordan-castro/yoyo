#![allow(non_camel_case_types)]

use std::ffi::c_char;

use etffi::{borrow_string, ptr_magic::PtrMagic};
use http::client::yoyonet_ClientResponse;

use crate::http::client::yoyonet_ClientOptions;

mod http;
mod utils;

/// http::client. Create a new yoyonet_ClientOptions.
/// 
/// return: OWNED
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_options_new() -> *mut yoyonet_ClientOptions {
    yoyonet_ClientOptions::new().into_raw()
}

/// http::client. Set the url of a yoyonet_ClientOptions
/// 
/// ptr: BORROW
/// url: BORROW
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_options_seturl(ptr: *mut yoyonet_ClientOptions, url: *const c_char) {
    if ptr.is_null() || url.is_null() {
        return;
    }

    let bo = unsafe { yoyonet_ClientOptions::from_borrow(ptr) };
    let burl = borrow_string!(url).to_string();

    bo.url = burl;
}

/// http::client. Set a header on a yoyonet_ClientOptions. It is appended to the map of headers.
/// If you want to remove a header you need to pass in key = nullptr.
/// 
/// ptr: BORROW
/// key: BORROW
/// value: BORROW
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_options_setheader(ptr: *mut yoyonet_ClientOptions, key: *const c_char, value: *const c_char) {
    if ptr.is_null() || key.is_null() {
        return;
    }

    let bo = unsafe { yoyonet_ClientOptions::from_borrow(ptr) };
    let key = borrow_string!(key).to_string();
    if value.is_null() {
        bo.headers.remove(&key);
    }

    let value = borrow_string!(value).to_string();
    bo.headers.insert(key, value);
}

/// http::client. Set body.
/// 
/// ptr: BORROW
/// body: BORROW
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_options_setbody(ptr: *mut yoyonet_ClientOptions, body: *const c_char) {
    if ptr.is_null() || body.is_null() {
        return;
    }

    let bo = unsafe { yoyonet_ClientOptions::from_borrow(ptr) };
    let body = borrow_string!(body).to_string();

    bo.body = body;
}

/// http::client. Set version.
/// 
/// ptr: BORROW
/// version: BORROW
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_options_setversion(ptr: *mut yoyonet_ClientOptions, version: i8) {
    if ptr.is_null() {
        return;
    }

    let bo = unsafe { yoyonet_ClientOptions::from_borrow(ptr) };
    bo.version = Some(version);
}

/// http::client. Make a request response from options.
/// 
/// options: BORROW
/// return: OWNED
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_request(options: *mut yoyonet_ClientOptions) -> *mut yoyonet_ClientResponse {
    if options.is_null() {
        return core::ptr::null_mut();
    }

    let options = unsafe { yoyonet_ClientOptions::from_borrow(options) };

    let response = http::client::request_with_options(options);
    match response {
        Ok(r) => r.into_raw(),
        Err(e) => {
            eprintln!("{e}");
            core::ptr::null_mut()
        }
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

/// http::client. Free a ClientOptions
///
/// ptr: TRANSFER
#[unsafe(no_mangle)]
pub extern "C" fn yoyonet_httpclient_options_free(ptr: *mut yoyonet_ClientOptions) {
    if ptr.is_null() {
        return;
    }

    let _ = yoyonet_ClientOptions::from_raw(ptr);
}
