/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

int main ()
{
    int sockfd;
    socklen_t len;
    struct sockaddr_in address;
    int result;

    /*  Create a socket for the client.  */

    sockfd = socket (AF_INET, SOCK_STREAM, 0);

    /*  Name the socket, as agreed with the server.  */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr ("127.0.0.1");
    address.sin_port = htons (9734);
    len = sizeof (address);

    /*  Now connect our socket to the server's socket.  */

    result = connect (sockfd, (struct sockaddr *) &address, len);

    if (result == -1)
    {
        perror ("oops: netclient");
        exit (1);
    }

    /*  We can now read/write via sockfd.  */
    date_query date_query_message;
    date_query_message.header[0] = 0;
    date_query_message.header[1] = 0;
    date_query_message.header[2] = 0;
    date_query_message.header[3] = 2;
    date_query_message.request_id = 1;
    char* date_frame = malloc(sizeof(char) * 200);
    size_t bytes_to_send_date = assemble_date_query_frame(date_query_message, date_frame);

    sqrt_message sqrt_query_message;
    sqrt_query_message.header[0] = 0;
    sqrt_query_message.header[1] = 0;
    sqrt_query_message.header[2] = 0;
    sqrt_query_message.header[3] = 1;
    sqrt_query_message.request_id = 2;
    sqrt_query_message.number = 16;
    char* sqrt_frame = malloc(sizeof(char) * 200);
    size_t bytes_to_send_sqrt = assemble_sqrt_frame(sqrt_query_message, sqrt_frame);

    write (sockfd, date_frame, bytes_to_send_date);
    write (sockfd, sqrt_frame, bytes_to_send_sqrt);

    while (1) {
        enum header_type headerType = get_header(sockfd);
        int req_id = get_req_id(sockfd);
        if (req_id == 1) {
            int length = get_length(sockfd);
            char *buf = malloc(length * sizeof(char));
            read(sockfd, buf, length);
            printf("%s", buf);
            free(buf);
        }

        if (req_id == 2) {
            size_t  data_size = sizeof(double);
            char* buf = malloc(data_size);
            read(sockfd, buf, data_size);
            sqrt_data data = retrieve_sqrt_data(buf);
            double sqrt = data.number;
            printf("fff\n");
            printf("%lf\n", sqrt);
            printf("Wydrukowano");
            free(buf);
        }
    }

    free(sqrt_frame);
    free(date_frame);
    close (sockfd);
    exit (0);
}