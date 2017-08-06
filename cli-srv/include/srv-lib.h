#ifndef __SRV_LIB_H
#define __SRV_LIB_H

#include <proto.h>

void usage(char* app_name);
void read_req_body(int sock, svc_request_body_t* body);
void read_req(int sock, request_t* req);
void fill_resp(responce_t* resp);
void write_resp(int sock, responce_t* resp);
void clean_resp(responce_t* resp);

int create_srv_connection(int port);
int accept_req(int sock);

#endif // __SRV_LIB_H