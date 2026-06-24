use std::{collections::HashMap, time::Duration};

use etffi::ptr_magic::PtrMagic;
use reqwest::blocking::Response;

use crate::{unwrap_result, utils::YResult};

#[repr(C)]
/// A wrapper around `reqwest::blocking::Response`
pub struct yoyonet_ClientResponse {
    /// The status code yo!
    status: u16,
    /// The response bytes yo! Precompiled for yoyos nasty ass.
    /// In C++ land there is a to_string method that uses these bytes too.
    bytes: *mut yoyonet_ClientBytes,
    /// Version, exposed already to yoyo_net.h
    version: i8,
}

impl PtrMagic for yoyonet_ClientResponse {}

#[repr(C)]
/// Simply wraps bytes
pub struct yoyonet_ClientBytes {
    bytes: *mut u8,
    size: usize,
}

impl yoyonet_ClientBytes {
    /// Make a new result from `Bytes`.
    ///
    /// It will copy the memory over to a owned pointer.
    ///
    /// On `drop` it will be freed.
    pub fn new(ptr: *const u8, size: usize) -> YResult<Self> {
        let layout = unwrap_result!(std::alloc::Layout::from_size_align(size, 1));
        let bytes: *mut u8 = unsafe { std::alloc::alloc(layout) };
        if bytes.is_null() {
            return Err("Bytes could not be allocated.".to_string());
        }

        // Copy mem over.
        unsafe {
            std::ptr::copy_nonoverlapping(ptr, bytes, size);
        }

        Ok(yoyonet_ClientBytes { bytes, size })
    }
}

impl Drop for yoyonet_ClientBytes {
    fn drop(&mut self) {
        if self.bytes.is_null() {
            return;
        }

        unsafe {
            std::alloc::dealloc(
                self.bytes,
                std::alloc::Layout::from_size_align(self.size, 1).unwrap(),
            );
        }

        self.bytes = std::ptr::null_mut();
        self.size = 0;
    }
}

impl PtrMagic for yoyonet_ClientBytes {}

pub const YOYONET_HTTP_VERSION_09: i8 = 0;
pub const YOYONET_HTTP_VERSION_10: i8 = 1;
pub const YOYONET_HTTP_VERSION_11: i8 = 2;
pub const YOYONET_HTTP_VERSION_2: i8 = 3;
pub const YOYONET_HTTP_VERSION_3: i8 = 4;

/// Convert a `reqwest::Version` into a integer
fn rversion_2_version(version: reqwest::Version) -> i8 {
    if version == reqwest::Version::HTTP_09 {
        YOYONET_HTTP_VERSION_09
    } else if version == reqwest::Version::HTTP_10 {
        YOYONET_HTTP_VERSION_10
    } else if version == reqwest::Version::HTTP_11 {
        YOYONET_HTTP_VERSION_11
    } else if version == reqwest::Version::HTTP_2 {
        YOYONET_HTTP_VERSION_2
    } else if version == reqwest::Version::HTTP_3 {
        YOYONET_HTTP_VERSION_3
    } else {
        -1
    }
}

/// Convert a `yoyonet_HttpVersion` into a `reqwest::Version`
fn version_2_rversion(version: i8) -> Option<reqwest::Version> {
    if version == YOYONET_HTTP_VERSION_09 {
        Some(reqwest::Version::HTTP_09)
    } else if version == YOYONET_HTTP_VERSION_10 {
        Some(reqwest::Version::HTTP_10)
    } else if version == YOYONET_HTTP_VERSION_11 {
        Some(reqwest::Version::HTTP_11)
    } else if version == YOYONET_HTTP_VERSION_2 {
        Some(reqwest::Version::HTTP_2)
    } else if version == YOYONET_HTTP_VERSION_3 {
        Some(reqwest::Version::HTTP_3)
    } else {
        None
    }
}

impl yoyonet_ClientResponse {
    /// Create a ClientResponse from `reqwest::blocking::Response`.
    pub fn new(response: Response) -> YResult<Self> {
        let status = response.status().as_u16();
        let version = rversion_2_version(response.version());
        let bytes = unwrap_result!(response.bytes());
        let bytes = yoyonet_ClientBytes::new(bytes.as_ptr(), bytes.len())?.into_raw();
        Ok(yoyonet_ClientResponse {
            status,
            bytes,
            version,
        })
    }
}

impl Drop for yoyonet_ClientResponse {
    fn drop(&mut self) {
        if self.bytes.is_null() {
            return;
        }

        let _ = yoyonet_ClientBytes::from_raw(self.bytes);
        self.bytes = std::ptr::null_mut();
    }
}

#[derive(Debug)]
/// Request types
#[repr(C)]
pub enum yoyonet_ClientRequestType {
    GET,
    POST,
    PATCH,
    DELETE,
    PUT,
}

#[derive(Debug)]
/// Options for a `get`.
pub struct yoyonet_ClientOptions {
    /// The url we are sending to.
    pub url: String,
    /// Optional headers.
    pub headers: HashMap<String, String>,
    /// What kind of request
    pub request_type: yoyonet_ClientRequestType,
    /// A body (as string)
    pub body: String,
    /// A optional Version
    pub version: Option<i8>,
    /// Set a Timeout
    pub timeout: u64,
}

impl PtrMagic for yoyonet_ClientOptions {}

impl yoyonet_ClientOptions {
    pub fn new() -> Self {
        Self {
            url: String::new(),
            headers: HashMap::new(),
            request_type: yoyonet_ClientRequestType::GET,
            body: String::new(),
            version: None,
            timeout: 60,
        }
    }
}

/// Make a Get request.
///
/// This spawns a client on the fly.
pub fn get(url: String) -> YResult<yoyonet_ClientResponse> {
    let result = unwrap_result!(reqwest::blocking::get(url));
    yoyonet_ClientResponse::new(result)
}

/// Make a request.
///
/// This spawns a client on the fly.
///
/// This is the same as `get` but with specific options that are controlled via `C`
pub fn request_with_options(options: &yoyonet_ClientOptions) -> YResult<yoyonet_ClientResponse> {
    let client = reqwest::blocking::Client::new();
    let url = &options.url;
    let mut rb = match options.request_type {
        yoyonet_ClientRequestType::GET => client.get(url),
        yoyonet_ClientRequestType::POST => client.post(url),
        yoyonet_ClientRequestType::PATCH => client.patch(url),
        yoyonet_ClientRequestType::DELETE => client.delete(url),
        yoyonet_ClientRequestType::PUT => client.put(url),
    };
    for h in options.headers.iter() {
        rb = rb.header(h.0, h.1);
    }

    if !options.body.is_empty() {
        rb = rb.body(options.body.clone());
    }

    if let Some(v) = options.version {
        let version = version_2_rversion(v);
        if let Some(rv) = version {
            rb = rb.version(rv);
        }
    }

    rb = rb.timeout(Duration::new(options.timeout, 0));

    // TODO: Authentication
    // rb = rb.a
    let res = rb.send();
    let result = unwrap_result!(res);

    yoyonet_ClientResponse::new(result)
}
