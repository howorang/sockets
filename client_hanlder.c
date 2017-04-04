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

void handle_date_query(int descriptor, int id);

void* client_handler(void* args) {
    int socket_file_descriptor = ((client_handler_args*) args)->socket_file_descriptor;
    struct sockaddr* client_socket_addr =((client_handler_args*) args)->client_socket_addr;

    while (1) {
        enum header_type headerType = get_header(socket_file_descriptor);
        int req_id = get_req_id(socket_file_descriptor);
        switch (headerType) {
            case MALFORMED_HEADER:
                printf("MALFORMED_HEADER\n");
                break;
            case SQRT_QUERY:
                printf("SQRT_QUERY\n");
                handle_sqrt_query(socket_file_descriptor, req_id);
                break;

            case SQRT_RESPONSE:
                printf("SQRT_RESPONSE\n");
                break;

            case DATE_QUERY:
                printf("DATE_QUERY\n");
                handle_date_query(socket_file_descriptor, req_id);
                break;

            case DATE_RESPONSE:
                printf("DATE_RESPONSE\n");
                break;
        }
    }
}

void handle_date_query(int socket_fd, int request_id) {
    int date_string_length;
    char* current_time = malloc(sizeof(char) * 50);
    date_string_length = get_current_time_string(current_time);
    date_response response;

    response.header[0] = 1;
    response.header[1] = 0;
    response.header[2] = 0;
    response.header[3] = 2;
    response.request_id = request_id;
    response.date_string = current_time;
    response.length = date_string_length;

    char* frame = malloc(sizeof(char) * 200); // outside alloc :(
    int bytes_to_send = assemble_date_response_frame(response, frame);
    write(socket_fd, frame, bytes_to_send);
    free(current_time);
    free(frame);
}

void handle_sqrt_query(int socket_fd, int request_id) {
    size_t  data_size = sizeof(double);
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
    message.request_id = request_id;
    message.number = result;
    char* frame = malloc(sizeof(char) * 200);
    size_t bytes_to_send = assemble_sqrt_frame(message, frame);
    write(socket_fd, frame, bytes_to_send);
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