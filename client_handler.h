//
// Created by howorang on 22.03.17.
//

#ifndef SOCKETS_CLIENT_HANDLER_H
#define SOCKETS_CLIENT_HANDLER_H

typedef struct client_handler_args {
    int socket_file_descriptor;
    struct sockaddr* client_socket_addr;
} client_handler_args;


void* client_handler(void* args);
void handle_sqrt_query(int socket_fd, int request_id);
void handle_date_query(int socket_fd, int request_id);
int get_current_time_string(char* string);
#endif //SOCKETS_CLIENT_HANDLER_H
