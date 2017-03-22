//
// Created by howorang on 21.03.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <netinet/in.h>
#include "endianness.h"
#include "protocol.h"


enum header_type get_header_type(int* header) {
    if(header[1] != 0 || header[2] != 0) {
        return MALFORMED_HEADER;
    }
    
    if(header [3] == 1) {
        if(header[0] == 0) {
            return SQRT_QUERY;  
        } else if (header[0] == 0) {
            return SQRT_RESPONSE;
        }
    }

    if(header [3] == 2) {
        if(header[0] == 0) {
            return DATE_QUERY;
        } else if (header[0] == 0) {
            return DATE_QUERY;
        }
    }

    return MALFORMED_HEADER;
}

char* assemble_sqrt_frame(sqrt_message message) {
    int* header = message.header;
    int request_id = message.request_id;
    double number = message.number;
    int converted_header[4];
    int converted_req_id = htonl(request_id);
    uint64_t converted_number = htond(number);
    for (int i = 0; i < 4; ++i) {
        converted_header[i] = htonl(header[i]);
    }


    char* frame = calloc(1, sizeof(int) * 4 + sizeof(int) + sizeof(double));
    char* frame_traverser = frame;
    memcpy(frame_traverser, converted_header, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;
    memcpy(frame_traverser, &converted_req_id, sizeof(int));
    frame_traverser += sizeof(int);
    memcpy(frame_traverser, &converted_number, sizeof(double));
    return frame;
}

sqrt_data retrieve_sqrt_data(char* frame) {
    char* frame_traverser = frame;
    int request_id;
    uint64_t number;
    sqrt_data data;

    memcpy(&request_id, frame_traverser, sizeof(int));
    frame_traverser += sizeof(int);

    memcpy(&number, frame_traverser, sizeof(double));

    data.request_id = ntohl(request_id);
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

char* assemble_date_query_frame(date_query query) {
    int* header = query.header;
    int request_id = query.request_id;
    int converted_header[4];
    int converted_req_id = htonl(request_id);
    for (int i = 0; i < 4; ++i) {
        converted_header[i] = htonl(header[i]);
    }

    char* frame = calloc(1, sizeof(int) * 4 + sizeof(int));
    char* frame_traverser = frame;
    memcpy(frame_traverser, converted_header, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;
    memcpy(frame_traverser, &converted_req_id, sizeof(int));
    return frame;
}

char* assemble_date_response_frame(date_response response) {
    int* header = response.header;
    int request_id = response.request_id;
    int converted_header[4];
    int converted_req_id = htonl(request_id);
    for (int i = 0; i < 4; ++i) {
        converted_header[i] = htonl(header[i]);
    }

    size_t converted_length = htonl(response.length);


    char* frame = calloc(1, sizeof(int) * 4 + sizeof(int) + sizeof(size_t) + response.length);
    char* frame_traverser = frame;

    memcpy(frame_traverser, converted_header, sizeof(int) * 4);
    frame_traverser += sizeof(int) * 4;

    memcpy(frame_traverser, &converted_req_id, sizeof(int));
    frame_traverser += sizeof(int);

    memcpy(frame_traverser, &converted_length, sizeof(size_t));
    frame_traverser += sizeof(size_t);

    memcpy(frame_traverser, response.date_string, response.length);
    return frame;
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