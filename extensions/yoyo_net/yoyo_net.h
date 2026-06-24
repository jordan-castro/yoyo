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
 * Options for a `get`.
 */
typedef struct yoyonet_ClientOptions yoyonet_ClientOptions;

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
 * http::client. Create a new yoyonet_ClientOptions.
 * 
 * return: OWNED
 */
struct yoyonet_ClientOptions *yoyonet_httpclient_options_new(void);

/**
 * http::client. Set the url of a yoyonet_ClientOptions
 * 
 * ptr: BORROW
 * url: BORROW
 */
void yoyonet_httpclient_options_seturl(struct yoyonet_ClientOptions *ptr, const char *url);

/**
 * http::client. Set a header on a yoyonet_ClientOptions. It is appended to the map of headers.
 * If you want to remove a header you need to pass in key = nullptr.
 * 
 * ptr: BORROW
 * key: BORROW
 * value: BORROW
 */
void yoyonet_httpclient_options_setheader(struct yoyonet_ClientOptions *ptr,
                                          const char *key,
                                          const char *value);

/**
 * http::client. Set body.
 * 
 * ptr: BORROW
 * body: BORROW
 */
void yoyonet_httpclient_options_setbody(struct yoyonet_ClientOptions *ptr, const char *body);

/**
 * http::client. Set version.
 * 
 * ptr: BORROW
 * version: BORROW
 */
void yoyonet_httpclient_options_setversion(struct yoyonet_ClientOptions *ptr, int8_t version);

/**
 * http::client. Make a request response from options.
 * 
 * url: BORROW
 * return: OWNED
 */
struct yoyonet_ClientResponse *yoyonet_httpclient_request(struct yoyonet_ClientOptions *options);

/**
 * http::client. free a ClientResponse.
 * 
 * ptr: TRANSFER
 */
void yoyonet_httpclient_free(struct yoyonet_ClientResponse *ptr);

/**
 * http::client. Free a ClientOptions
 *
 * ptr: TRANSFER
 */
void yoyonet_httpclient_options_free(struct yoyonet_ClientOptions *ptr);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  /* YOYONET_H */
