#include <stdlib.h>

#include <proto.h>
#include <trace.h>
#include <cli-lib.h>

void print_responce(responce_t* resp)
{
    switch (resp->body.ok.return_code)
    {
    case CUBE_OAUTH2_ERR_OK:
        fprintf(stdout, "client_id: %s\n", resp->body.ok.client_id.str);
        fprintf(stdout, "client_type: %d\n", resp->body.ok.client_type);
        fprintf(stdout, "expires_in: %d\n", resp->body.ok.expires_in);
        fprintf(stdout, "user_id: 101010\n");
        fprintf(stdout, "username: %s\n", resp->body.ok.username.str);
        break;
    case CUBE_OAUTH2_ERR_TOKEN_NOT_FOUND:
        fprintf(stdout, "error: CUBE_OAUTH2_ERR_TOKEN_NOT_FOUND\n");
        fprintf(stdout, "message: bad scope\n");
        break;
    case CUBE_OAUTH2_ERR_DB_ERROR:
        fprintf(stdout, "error: CUBE_OAUTH2_ERR_DB_ERROR\n");
        fprintf(stdout, "message: db error\n");
        break;
    case CUBE_OAUTH2_ERR_UNKNOWN_MSG:
        fprintf(stdout, "error: CUBE_OAUTH2_ERR_UNKNOWN_MSG\n");
        fprintf(stdout, "message: unknown svc message type\n");
        break;
    case CUBE_OAUTH2_ERR_BAD_PACKET:
        fprintf(stdout, "error: CUBE_OAUTH2_ERR_BAD_PACKET\n");
        fprintf(stdout, "message: bad packet\n");
        break;
    case CUBE_OAUTH2_ERR_BAD_CLIENT:
        fprintf(stdout, "error: CUBE_OAUTH2_ERR_BAD_CLIENT\n");
        fprintf(stdout, "message: bad client\n");
        break;
    case CUBE_OAUTH2_ERR_BAD_SCOPE:
        fprintf(stdout, "error: CUBE_OAUTH2_ERR_BAD_SCOPE\n");
        fprintf(stdout, "message: bad scope\n");
        break;
    default:
        fprintf(stdout, "unknown return_code\n");
    }
}

void __real_read_resp(int sock, responce_t* resp);

void __wrap_read_resp(int sock, responce_t* resp)
{
    __real_read_resp(sock, resp);
    print_responce(resp);
}
