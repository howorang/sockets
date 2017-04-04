//
// Created by howorang on 21.03.17.
//

#ifndef SOCKETS_PROTOCOL_H
#define SOCKETS_PROTOCOL_H

typedef struct sqrt_message {
    int header[4];
    int request_id;
    double number;
} sqrt_message;

typedef struct sqrt_data {
    double number;
} sqrt_data;

typedef struct date_query {
    int header[4];
    int request_id;
} date_query;

typedef struct date_response {
    int header[4];
    int request_id;
    size_t length;
    char* date_string;
} date_response;


enum header_type {
    MALFORMED_HEADER = 0,
    SQRT_QUERY = 1,
    SQRT_RESPONSE = 2,
    DATE_QUERY = 3,
    DATE_RESPONSE = 4
};

enum header_type get_header_type(int* header);
size_t assemble_sqrt_frame(sqrt_message message, char* frame);
sqrt_data retrieve_sqrt_data(char* frame);
sqrt_message retrieve_sqrt_message(char* frame);
size_t assemble_date_query_frame(date_query query, char* frame);
size_t assemble_date_response_frame(date_response response, char* frame);
date_response retrieve_date_response(char* frame);
enum header_type get_header(int socketfd);
int get_req_id(int socketfd);
size_t get_length(int socketfd);
#endif //SOCKETS_PROTOCOL_H
