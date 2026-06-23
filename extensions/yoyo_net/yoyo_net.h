// SPDX-License-Identifier: MIT

#ifndef YOYONET_H
#define YOYONET_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define YOYONET_HTTP_VERSION_09 0

#define YOYONET_HTTP_VERSION_10 1

#define YOYONET_HTTP_VERSION_11 2

#define YOYONET_HTTP_VERSION_2 3

#define YOYONET_HTTP_VERSION_3 4

/**
 * Simply wraps bytes
 */
typedef struct yoyonet_ClientBytes {
  uint8_t *bytes;
  uintptr_t size;
} yoyonet_ClientBytes;

/**
 * A wrapper around `reqwest::blocking::Response`
 */
typedef struct yoyonet_ClientResponse {
  /**
   * The status code yo!
   */
  uint16_t status;
  /**
   * The response bytes yo! Precompiled for yoyos nasty ass.
   * In C++ land there is a to_string method that uses these bytes too.
   */
  struct yoyonet_ClientBytes *bytes;
  /**
   * Version, exposed already to yoyo_net.h
   */
  int8_t version;
} yoyonet_ClientResponse;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * http::client. Make a get response from just URL.
 * 
 * url: BORROW
 * return: OWNED
 */
struct yoyonet_ClientResponse *yoyonet_httpclient_get(const char *url);

/**
 * http::client. free a ClientResponse.
 * 
 * ptr: TRANSFER
 */
void yoyonet_httpclient_free(struct yoyonet_ClientResponse *ptr);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  /* YOYONET_H */
