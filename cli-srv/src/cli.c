#include <stdlib.h>

#include <proto.h>
#include <trace.h>
#include <cli-lib.h>

int main(int argc, char** argv)
{
    if (argc < 4)
        usage(argv[0]);

    char* host  = argv[1];
    int   port  = atoi(argv[2]);
    char* token = argv[3];
    char* scope = argv[4];

    int sock = create_cli_connection(host, port);

    request_t* req = malloc(sizeof(request_t));
    fill_req(req, token, scope);
    write_req(sock, req);
    clean_req(req);
    free(req);

    responce_t* resp = malloc(sizeof(responce_t));
    read_resp(sock, resp);
    clean_resp(resp);
    free(resp);

    close_cli_connection(sock);

    return 0;
}