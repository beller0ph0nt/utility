#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <proto.h>
#include <trace.h>
#include <srv-lib.h>
#include <common-lib.h>

void read_req_body(int sock, svc_request_body_t* body)
{
    read(sock, &body->svc_msg, sizeof(int32_t));
    read_string(sock, &body->token);
    read_string(sock, &body->scope);
}

void read_req(int sock, request_t* req)
{
    TRACE_DEBUG("read request");
    read(sock, &req->header, sizeof(header_t));
    read_req_body(sock, &req->body);

    TRACE_DEBUG("  header");
    TRACE_DEBUG("    svc_id: %d", req->header.svc_id);
    TRACE_DEBUG("    body_length: %d", req->header.body_length);
    TRACE_DEBUG("    request_id: %d", req->header.request_id);
    TRACE_DEBUG("  body");
    TRACE_DEBUG("    svc_msg: %d", req->body.svc_msg);
    TRACE_DEBUG("    token len: %d str: %s", req->body.token.len, req->body.token.str);
    TRACE_DEBUG("    scope len: %d str: %s", req->body.scope.len, req->body.scope.str);
}

void fill_resp(responce_t* resp)
{
    TRACE_DEBUG("fill responce");
    resp->body.ok.return_code = CUBE_OAUTH2_ERR_OK;
    fill_string(&resp->body.ok.client_id, "test_client_id");
    resp->body.ok.client_type = 2002;
    fill_string(&resp->body.ok.username, "testuser@mail.ru");
    resp->body.ok.expires_in = 3600;

    resp->header.svc_id = CUBE_OAUTH2_SVC_ID;
    resp->header.body_length =
        sizeof(resp->body.ok.return_code) +
        sizeof(resp->body.ok.client_id.len) + resp->body.ok.client_id.len +
        sizeof(resp->body.ok.client_type) +
        sizeof(resp->body.ok.username.len) + resp->body.ok.username.len +
        sizeof(resp->body.ok.expires_in);
    resp->header.request_id = 13;
}

void clean_resp(responce_t* resp)
{
    TRACE_DEBUG("clean responce");
    memset(&resp->header, 0, sizeof(header_t));

    resp->body.ok.return_code = 0;
    clean_string(&resp->body.ok.client_id);
    resp->body.ok.client_type = 0;
    clean_string(&resp->body.ok.username);
    resp->body.ok.expires_in = 0;
}

void write_resp(int sock, responce_t* resp)
{
    TRACE_DEBUG("write responce");
    write(sock, &resp->header,                sizeof(header_t));
    write(sock, &resp->body.ok.return_code,   sizeof(int32_t));
    write(sock, &resp->body.ok.client_id.len, sizeof(int32_t));
    write(sock,  resp->body.ok.client_id.str, resp->body.ok.client_id.len);
    write(sock, &resp->body.ok.client_type,   sizeof(int32_t));
    write(sock, &resp->body.ok.username.len,  sizeof(int32_t));
    write(sock,  resp->body.ok.username.str,  resp->body.ok.username.len);
    write(sock, &resp->body.ok.expires_in,    sizeof(int32_t));
}

/*
void usage(char* app_name)
{
    fprintf(stderr,"usage: %s <port_number>\n", app_name);
    exit(EXIT_FAILURE);
}
*/

int main(int argc, char** argv)
{
    if (argc < 2)
        usage(argv[0]);

    int port = atoi(argv[1]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0)
    {
        TRACE_ERROR("socket() failed: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    struct sockaddr_in serv_addr;
    memset((char*) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(port);

    if (bind(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        TRACE_ERROR("bind() failed: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    TRACE_DEBUG("listening...");
    listen(sock, 1);

    TRACE_DEBUG("accepting request...");
    struct sockaddr_in cli_addr;
    socklen_t sock_len = sizeof(cli_addr);
    int newsock = accept(sock, (struct sockaddr *) &cli_addr, &sock_len);
    if (newsock < 0)
    {
        TRACE_ERROR("accept() failed: %s", strerror(errno));
        return EXIT_FAILURE;
    }

    request_t* req = malloc(sizeof(request_t));
    read_req(newsock, req);
    free(req);

    responce_t* resp = malloc(sizeof(responce_t));
    fill_resp(resp);
    write_resp(newsock, resp);
    clean_resp(resp);
    free(resp);

    close(newsock);

    close(sock);

    return 0;
}