use etffi::{ptr_magic::PtrMagic};
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
    version: i8
}

impl PtrMagic for yoyonet_ClientResponse {}

#[repr(C)]
/// Simply wraps bytes
pub struct yoyonet_ClientBytes {
    bytes: *mut u8,
    size:  usize
}

impl yoyonet_ClientBytes {
    // pub fn new_empty() -> Self {
    //     ClientBytes { bytes: std::ptr::null_mut(), size: 0 }
    // }
    pub fn new(ptr: *const u8, size: usize) -> YResult<Self> {
        let layout = unwrap_result!(std::alloc::Layout::from_size_align(size, 1));
        let bytes: *mut u8 = unsafe{ std::alloc::alloc(layout) };
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
            std::alloc::dealloc(self.bytes, std::alloc::Layout::from_size_align(self.size, 1).unwrap());
        }

        self.bytes = std::ptr::null_mut();
        self.size = 0;
    }
}

impl PtrMagic for yoyonet_ClientBytes {}

pub const YOYONET_HTTP_VERSION_09: i8 = 0;
pub const YOYONET_HTTP_VERSION_10: i8 = 1;
pub const YOYONET_HTTP_VERSION_11: i8 = 2;
pub const YOYONET_HTTP_VERSION_2:  i8 = 3;
pub const YOYONET_HTTP_VERSION_3:  i8 = 4;

/// Convert a `reqwest::Version` into a integer
fn version_2_version(version: reqwest::Version) -> i8 {
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

impl yoyonet_ClientResponse {
    pub fn new(response: Response) -> YResult<Self> {
        let status = response.status().as_u16();
        let version = version_2_version(response.version());
        let bytes = unwrap_result!(response.bytes());
        let bytes = yoyonet_ClientBytes::new(bytes.as_ptr(), bytes.len())?.into_raw();
        Ok(
            yoyonet_ClientResponse { 
                status,
                bytes,
                version
            }
        )
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

/// Make a Get request.
/// 
/// This spawns a client on the fly.
pub fn get(url: String) -> YResult<yoyonet_ClientResponse> {
    let result = unwrap_result!(reqwest::blocking::get(url));
    yoyonet_ClientResponse::new(result)
}
