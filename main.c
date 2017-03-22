#include <stdio.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "client_handler.h"

int main() {
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    server_sockfd = socket (AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl (INADDR_ANY);
    server_address.sin_port = htons (9734);
    server_len = sizeof (server_address);
    bind (server_sockfd, (struct sockaddr *) &server_address, server_len);

    /*  Create a connection queue, ignore child exit details and wait for clients.  */

    listen (server_sockfd, 5);
    char* string = malloc(sizeof(char) * 50);
    printf(get_current_time_string(string));

    while (1) {
        pthread_t current_thread;
        client_handler_args handler_args;

        printf("server waiting\n");

        /*  Accept connection.  */

        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd,
                               (struct sockaddr *) &client_address,
                               &client_len);

        //Accept blokuje do tego momentu

        handler_args.client_socket_addr = (struct sockaddr *) &client_address;
        handler_args.socket_file_descriptor = client_sockfd;

        pthread_create(&current_thread, NULL, &client_handler, &handler_args);
        pthread_detach(current_thread);
    }
}