#include <poll.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <trace.h>
#include <proto.h>
#include <cli-lib.h>
#include <common-lib.h>

typedef void (*body_reader_t)(int sock, responce_body_t* body);

void ok_body_reader(int sock, responce_body_t* body)
{
    TRACE_DEBUG("ok body reader");
    read_string(sock, &body->ok.client_id);
    read(sock, &body->ok.client_type, sizeof(int32_t));
    read_string(sock, &body->ok.username);
    read(sock, &body->ok.expires_in, sizeof(int32_t));
}

void err_body_reader(int sock, responce_body_t* body)
{
    TRACE_DEBUG("err body reader");
    read_string(sock, &body->err.error_string);
}

void empty_body_reader(int sock, responce_body_t* body)
{
    TRACE_DEBUG("empty body reader");
}

static const body_reader_t body_reader[CUBE_OAUTH2_ERR_MASK + 1] = {
    [CUBE_OAUTH2_ERR_OK]              = ok_body_reader,
    [CUBE_OAUTH2_ERR_TOKEN_NOT_FOUND] = err_body_reader,
    [CUBE_OAUTH2_ERR_DB_ERROR]        = err_body_reader,
    [CUBE_OAUTH2_ERR_UNKNOWN_MSG]     = err_body_reader,
    [CUBE_OAUTH2_ERR_BAD_PACKET]      = err_body_reader,
    [CUBE_OAUTH2_ERR_BAD_CLIENT]      = err_body_reader,
    [CUBE_OAUTH2_ERR_BAD_SCOPE]       = err_body_reader,
    empty_body_reader
};

void read_body(int sock, responce_body_t* body)
{
    read(sock, &body->ok.return_code, sizeof(int32_t));
    TRACE_DEBUG("    return code: %d", body->ok.return_code);
    body_reader[body->ok.return_code & CUBE_OAUTH2_ERR_MASK](sock, body);
}







void ok_body_cleaner(responce_body_t* body)
{
    TRACE_DEBUG("ok body cleaner");
    body->ok.return_code = 0;
    clean_string(&body->ok.client_id);
    body->ok.client_type = 0;
    clean_string(&body->ok.username);
    body->ok.expires_in = 0;
}

void err_body_cleaner(responce_body_t* body)
{
    TRACE_DEBUG("err body cleaner");
    clean_string(&body->err.error_string);
}

void empty_body_cleaner(responce_body_t* body)
{
    TRACE_DEBUG("empty body cleaner");
}

typedef void (*body_cleaner_t)(responce_body_t* body);

static const body_cleaner_t body_cleaner[CUBE_OAUTH2_ERR_MASK + 1] = {
    [CUBE_OAUTH2_ERR_OK]              = ok_body_cleaner,
    [CUBE_OAUTH2_ERR_TOKEN_NOT_FOUND] = err_body_cleaner,
    [CUBE_OAUTH2_ERR_DB_ERROR]        = err_body_cleaner,
    [CUBE_OAUTH2_ERR_UNKNOWN_MSG]     = err_body_cleaner,
    [CUBE_OAUTH2_ERR_BAD_PACKET]      = err_body_cleaner,
    [CUBE_OAUTH2_ERR_BAD_CLIENT]      = err_body_cleaner,
    [CUBE_OAUTH2_ERR_BAD_SCOPE]       = err_body_cleaner,
    empty_body_cleaner
};

void read_resp(int sock, responce_t* resp)
{
    TRACE_DEBUG("read responce %p", resp);
    read(sock, &resp->header, sizeof(header_t));
    TRACE_DEBUG("  header");
    TRACE_DEBUG("    svc_id: %d", resp->header.svc_id);
    TRACE_DEBUG("    body_length: %d", resp->header.body_length);
    TRACE_DEBUG("    request_id: %d", resp->header.request_id);
    TRACE_DEBUG("  body");
    read_body(sock, &resp->body);
}

void clean_resp(responce_t* resp)
{
    TRACE_DEBUG("clean responce");
    memset(&resp->header, 0, sizeof(header_t));
    body_cleaner[resp->body.ok.return_code & CUBE_OAUTH2_ERR_MASK](&resp->body);
}

void usage(char* app_name)
{
    printf("usage: %s <host> <port> <token> <scope>\n", app_name);
    exit(EXIT_FAILURE);
}

void fill_req(request_t* req, const char* token, const char* scope)
{
    TRACE_DEBUG("fill request %p", req);
    req->body.svc_msg = CUBE_OAUTH2_SVC_MSG;
    fill_string(&req->body.token, token);
    fill_string(&req->body.scope, scope);

    req->header.svc_id      = CUBE_OAUTH2_SVC_ID;
    req->header.body_length = sizeof(req->body.svc_msg) +
                              sizeof(req->body.token.len) + req->body.token.len +
                              sizeof(req->body.scope.len) + req->body.scope.len;
    req->header.request_id  = 0;

    TRACE_DEBUG("request");
    TRACE_DEBUG("  header");
    TRACE_DEBUG("    svc_id: %d", req->header.svc_id);
    TRACE_DEBUG("    body_length: %d", req->header.body_length);
    TRACE_DEBUG("    request_id: %d", req->header.request_id);
    TRACE_DEBUG("  body");
    TRACE_DEBUG("    svc_msg: %d", req->body.svc_msg);
    TRACE_DEBUG("    token len: %d str: %s", req->body.token.len, req->body.token.str);
    TRACE_DEBUG("    scope len: %d str: %s", req->body.scope.len, req->body.scope.str);
}

void write_req(int sock, request_t* req)
{
    TRACE_DEBUG("write request %p", req);
    write(sock, &req->header,         sizeof(header_t)   );
    write(sock, &req->body.svc_msg,   sizeof(int32_t)    );
    write(sock, &req->body.token.len, sizeof(int32_t)    );
    write(sock,  req->body.token.str, req->body.token.len);
    write(sock, &req->body.scope.len, sizeof(int32_t)    );
    write(sock,  req->body.scope.str, req->body.scope.len);
}

void clean_req(request_t* req)
{
    TRACE_DEBUG("clean request %p", req);
    memset(&req->header, 0, sizeof(header_t));
    req->body.svc_msg = 0;
    clean_string(&req->body.token);
    clean_string(&req->body.scope);
}

/*
#define BAD_ATTEMPT_COUNT  3
#define WAIT_TIMEOUT       3000

int recv_buffer(int fd, void* buf, int len)
{
    int attempt    = BAD_ATTEMPT_COUNT;
    int remain_len = len;
    int readed_len = 0;

    struct pollfd fds;
    fds.fd     = fd;
    fds.events = POLLIN;

    printf("reading...\n");

    do
    {
        printf("    attempt #%d remain: %d readed: %d\n",
               attempt, remain_len, readed_len);
        if (poll(&fds, 1, WAIT_TIMEOUT) > 0)
        {
            int r = read(fd, buf + readed_len, remain_len);
            if (r > 0)
            {
                readed_len += r;
                remain_len -= r;
            }
            else
                attempt--;
        }
        else
            attempt--;
    } while (attempt > 0 && remain_len != 0);

    if (readed_len != len)
        return -1;

    return 0;
}
*/

int create_cli_connection(char* host, int port)
{
    TRACE_DEBUG("open client connection");
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        TRACE_ERROR("socket() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct hostent* server = gethostbyname(host);
    if (server == NULL)
    {
        TRACE_ERROR("Host not found\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    strncpy((char*) &serv_addr.sin_addr.s_addr,
        (char*) server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        TRACE_ERROR("connect() failed: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return sock;
}

void close_cli_connection(int sock)
{
    TRACE_DEBUG("close client connection");
    close(sock);
}