//
// Created by howorang on 21.03.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include <unistd.h>
#include "endianness.h"
#include "protocol.h"


enum header_type get_header_type(int* header) {
    if(header[1] != 0 || header[2] != 0) {
        return MALFORMED_HEADER;
    }
    
    if(header [3] == 1) {
        if(header[0] == 0) {
            return SQRT_QUERY;  
        } else if (header[0] == 1) {
            return SQRT_RESPONSE;
        }
    }

    if(header [3] == 2) {
        if(header[0] == 0) {
            return DATE_QUERY;
        } else if (header[0] == 1) {
            return DATE_RESPONSE;
        }
    }

    return MALFORMED_HEADER;
}

size_t assemble_sqrt_frame(sqrt_message message, char* frame) {
    int* header = message.header;
    int request_id = message.request_id;
    double number = message.number;
    int converted_header[4];
    int converted_req_id = htonl(request_id);
    uint64_t converted_number = htond(number);
    for (int i = 0; i < 4; ++i) {
        converted_header[i] = htonl(header[i]);
    }


    size_t size_of_frame = sizeof(int) * 4 + sizeof(int) + sizeof(double);
    char* frame_traverser = frame;
    memcpy(frame_traverser, converted_header, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;
    memcpy(frame_traverser, &converted_req_id, sizeof(int));
    frame_traverser += sizeof(int);
    memcpy(frame_traverser, &converted_number, sizeof(double));
    return size_of_frame;
}

sqrt_data retrieve_sqrt_data(char* frame) {
    char* frame_traverser = frame;
    uint64_t number;
    sqrt_data data;

    memcpy(&number, frame_traverser, sizeof(double));

    data.number = ntohd(number);
    return data;
}

sqrt_message retrieve_sqrt_message(char* frame) {
    int header[4]; 
    char* frame_traverser = frame;
    int request_id;
    uint64_t number;
    sqrt_message data;

    memcpy(&header, frame_traverser, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;
    
    memcpy(&request_id, frame_traverser, sizeof(int));
    frame_traverser += sizeof(int);

    memcpy(&number, frame_traverser, sizeof(double));
    for (int i = 0; i < 4; ++i) {
        data.header[i] = ntohl(header[i]);
    }
    data.request_id = ntohl(request_id);
    data.number = ntohd(number);

    return data;
}

size_t assemble_date_query_frame(date_query query, char* frame) {
    int* header = query.header;
    int request_id = query.request_id;
    int converted_header[4];
    int converted_req_id = htonl(request_id);
    for (int i = 0; i < 4; ++i) {
        converted_header[i] = htonl(header[i]);
    }

    size_t size_of_frame = sizeof(int) * 4 + sizeof(int);
    char* frame_traverser = frame;
    memcpy(frame_traverser, converted_header, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;
    memcpy(frame_traverser, &converted_req_id, sizeof(int));
    return size_of_frame;
}

size_t assemble_date_response_frame(date_response response, char* frame) {
    int* header = response.header;
    int request_id = response.request_id;
    int converted_header[4];
    int converted_req_id = htonl(request_id);
    for (int i = 0; i < 4; ++i) {
        converted_header[i] = htonl(header[i]);
    }

    size_t converted_length = htonl(response.length);

    size_t size_of_frame = sizeof(int) * 4 + sizeof(int) + sizeof(size_t) + response.length;
    char* frame_traverser = frame;

    memcpy(frame_traverser, converted_header, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;

    memcpy(frame_traverser, &converted_req_id, sizeof(int));
    frame_traverser += sizeof(int);

    memcpy(frame_traverser, &converted_length, sizeof(size_t));
    frame_traverser += sizeof(size_t);

    memcpy(frame_traverser, response.date_string, response.length);
    return size_of_frame;
}

date_response retrieve_date_response(char* frame) {
    int header[4];
    char* frame_traverser = frame;
    int request_id;
    size_t length;
    char* date_string;
    date_response data;

    memcpy(&header, frame_traverser, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;

    memcpy(&request_id, frame_traverser, sizeof(int));
    frame_traverser += sizeof(int);

    memcpy(&length, frame_traverser, sizeof(size_t));
    frame_traverser += sizeof(size_t);

    length = ntohl(length);

    date_string = malloc(length);
    memcpy(date_string, frame_traverser, length);

    for (int i = 0; i < 4; ++i) {
        data.header[i] = ntohl(header[i]);
    }
    data.request_id = ntohl(request_id);
    data.length = length;
    data.date_string = date_string;
    return data;
}

enum header_type get_header(int socketfd) {
    int header_size = 4 * sizeof(int);
    int header_not_converted[4];
    int header[4];
    char* buf = malloc(header_size);
    char* frame_traverser = buf;
    read(socketfd, buf, header_size);
    memcpy(&header_not_converted, frame_traverser, header_size);

    for (int i = 0; i < 4; ++i) {
        header[i] = ntohl(header_not_converted[i]);
    }

    free(buf);
    return get_header_type(header);
}

int get_req_id(int socketfd) {
    int req_id;
    int req_id_not_converted;
    char* buf = malloc(sizeof(int));
    char* frame_traverser = buf;
    read(socketfd, buf, sizeof(int));
    memcpy(&req_id_not_converted, frame_traverser, sizeof(int));
    req_id = ntohl(req_id_not_converted);
    free(buf);
    return req_id;
}

size_t get_length(int socketfd) {
    size_t length;
    size_t length_not_converted;
    char* buf = malloc(sizeof(size_t));
    char* frame_traverser = buf;
    read(socketfd, buf, sizeof(size_t));
    memcpy(&length_not_converted, frame_traverser, sizeof(size_t));
    length = ntohl(length_not_converted);
    free(buf);
    return length;
}