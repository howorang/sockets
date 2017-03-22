#include <stddef.h>
#include <stdlib.h>
#include "client_handler.h"
#include "protocol.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include <time.h>
//
// Created by howorang on 22.03.17.
//

void* client_handler(void* args) {
    int socket_file_descriptor = ((client_handler_args*) args)->socket_file_descriptor;
    struct sockaddr* client_socket_addr =((client_handler_args*) args)->client_socket_addr;
    const size_t header_size = sizeof(int) * 4;
    char* buf = calloc(1, header_size);


    while (1) {
        int header[4];
        read(socket_file_descriptor, buf, header_size);
        memcpy(header, buf, header_size);
        for (int i = 0; i < 4; ++i) {
            header[i] = htonl(header[i]);
        }
        switch (get_header_type(header)) {
            case MALFORMED_HEADER:
                printf("MALFORMED_HEADER\n");
                break;
            case SQRT_QUERY:
                printf("SQRT_QUERY\n");
                handle_sqrt_query(socket_file_descriptor);
                break;

            case SQRT_RESPONSE:
                printf("SQRT_RESPONSE\n");
                break;

            case DATE_QUERY:
                printf("DATE_QUERY\n");
                break;

            case DATE_RESPONSE:
                printf("DATE_RESPONSE\n");
                break;
        }
    }
}

void handle_sqrt_query(int socket_fd) {
    size_t  data_size = sizeof(int) + sizeof(double);
    size_t frame_size = sizeof(int) * 4 + sizeof(int) + sizeof(double);
    char* buf = malloc(data_size);
    read(socket_fd, buf, data_size);
    sqrt_data data = retrieve_sqrt_data(buf);
    double result = sqrt(data.number);
    sqrt_message message;
    message.header[0] = 1;
    message.header[1] = 0;
    message.header[2] = 0;
    message.header[3] = 1;
    message.request_id = data.request_id;
    message.number = result;
    char* frame = assemble_sqrt_frame(message);
    write(socket_fd, frame, frame_size);
    free(buf);
    free(frame);
}

int get_current_time_string(char* string) {
    time_t rawtime;
    struct tm * timeinfo;
    int saved_bytes;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    saved_bytes = sprintf ( string ,"%s", asctime (timeinfo) );
    return saved_bytes;

}