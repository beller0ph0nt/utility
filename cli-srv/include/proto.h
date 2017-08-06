#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <sys/types.h>

#define CUBE_OAUTH2_ERR_OK              0x00000000
#define CUBE_OAUTH2_ERR_TOKEN_NOT_FOUND 0x00000001
#define CUBE_OAUTH2_ERR_DB_ERROR        0x00000002
#define CUBE_OAUTH2_ERR_UNKNOWN_MSG     0x00000003
#define CUBE_OAUTH2_ERR_BAD_PACKET      0x00000004
#define CUBE_OAUTH2_ERR_BAD_CLIENT      0x00000005
#define CUBE_OAUTH2_ERR_BAD_SCOPE       0x00000006

#define CUBE_OAUTH2_ERR_MASK            0x00000007

#define CUBE_OAUTH2_SVC_ID              0x00000002
#define CUBE_OAUTH2_SVC_MSG             0x00000001

typedef __int8_t  int8_t;
typedef __int32_t int32_t;
typedef __int64_t int64_t;

typedef struct {
    int32_t len;
    int8_t* str;
} string_t;

typedef struct {
    int32_t svc_id;
    int32_t body_length;
    int32_t request_id;
} header_t;

typedef struct {
    int32_t  svc_msg;
    string_t token;
    string_t scope;
} svc_request_body_t;

typedef struct {
    int32_t  return_code;
    string_t client_id;
    int32_t  client_type;
    string_t username;
    int32_t  expires_in;
} svc_ok_responce_body_t;

typedef struct {
    int32_t  return_code;
    string_t error_string;
} svc_err_responce_body_t;

typedef union {
    svc_ok_responce_body_t  ok;
    svc_err_responce_body_t err;
} responce_body_t;

typedef struct {
    header_t           header;
    svc_request_body_t body;
} request_t;

typedef struct {
    header_t        header;
    responce_body_t body;
} responce_t;

typedef union {
    request_t  req;
    responce_t resp;
} packet_t;

#endif // __PROTOCOL_H
