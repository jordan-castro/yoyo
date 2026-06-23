use etffi::{bytes::CBytes, ptr_magic::PtrMagic};
use reqwest::blocking::Response;

use crate::{unwrap_result, utils::YResult};

/// A wrapper around `reqwest::blocking::Response`
#[repr(C)]
pub struct ClientResponse {
    /// The internal response. Should not be accessed outside of `yoyonet`.
    _response: Response,
    status: u16,
}

impl PtrMagic for ClientResponse {}

impl ClientResponse {
    pub fn new(response: Response) -> YResult<Self> {
        let status = response.status().as_u16();
        Ok(
            ClientResponse { 
                _response: response, 
                status: status 
            }
        )
    }

    /// Get the bytes but as a `CBytes` object.
    pub fn get_bytes(&self) -> YResult<CBytes> {
        let bytes = unwrap_result!(self._response.bytes());
        let p = bytes.as_ptr();
    }
}

/// Make a Get request.
/// 
/// This spawns a client on the fly.
pub fn get(url: String) -> YResult<ClientResponse> {
    let result = unwrap_result!(reqwest::blocking::get(url));
    ClientResponse::new(result)
}
