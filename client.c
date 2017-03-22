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
    char ch = 'A';

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
    sqrt_message query_message;
    query_message.header[0] = 0;
    query_message.header[1] = 0;
    query_message.header[2] = 0;
    query_message.header[3] = 1;
    query_message.request_id = 1;
    query_message.number = 4;
    char* frame = assemble_sqrt_frame(query_message);
    size_t  size_of_frame = sizeof(int) * 4 + sizeof(int) + sizeof(double);
    write (sockfd, frame, size_of_frame);

    char *message_frame_buf = malloc(size_of_frame);
    read(sockfd, message_frame_buf, size_of_frame);
    sqrt_message response_message = retrieve_sqrt_message(message_frame_buf);
    printf("%f",response_message.number);

    free(frame);
    free(message_frame_buf);
    close (sockfd);
    exit (0);
}