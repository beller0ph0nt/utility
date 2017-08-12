#include <stdlib.h>
#include <sys/socket.h>

#include <trace.h>
#include <srv-lib.h>

int main(int argc, char** argv)
{
    if (argc < 2)
        usage(argv[0]);

    int port = atoi(argv[1]);
    int sock = create_srv_connection(port);

//    while (1)
    {
        int newsock = accept_req(sock);

        request_t* req = malloc(sizeof(request_t));
        read_req(newsock, req);
        free(req);

        responce_t* resp = malloc(sizeof(responce_t));
        fill_resp(resp);
        write_resp(newsock, resp);
        clean_resp(resp);
        free(resp);

        shutdown(newsock, SHUT_RDWR);
        close(newsock);
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);

    return 0;
}