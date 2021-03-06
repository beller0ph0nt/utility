#ifndef __CLI_LIB_H
#define __CLI_LIB_H

#include <proto.h>

void usage(char* app_name);

void ok_body_reader(int sock, responce_body_t* body);
void err_body_reader(int sock, responce_body_t* body);
void empty_body_reader(int sock, responce_body_t* body);
void read_resp(int sock, responce_t* resp);

void ok_body_cleaner(responce_body_t* body);
void err_body_cleaner(responce_body_t* body);
void empty_body_cleaner(responce_body_t* body);
void clean_resp(responce_t* resp);

void fill_req(request_t* req, const char* token, const char* scope);
void write_req(int sock, request_t* req);
void clean_req(request_t* req);

int  create_cli_connection(char* host, int port);
void close_cli_connection(int sock);

#endif // __CLI_LIB_H