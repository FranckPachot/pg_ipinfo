#include "postgres.h"
#include "fmgr.h"
#include "utils/builtins.h"


#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // Include this for the close() function
#define BUFFER_SIZE 1024

PG_FUNCTION_INFO_V1( getipinfo );

Datum getipinfo( PG_FUNCTION_ARGS ) 
{

    const char *host = "ipinfo.io";
    const char *path = text_to_cstring(PG_GETARG_TEXT_P(0));

    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        perror("Error: Couldn't resolve hostname");
        return 1;
    }
    struct sockaddr_in server_address;
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(80);  // HTTP port
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error: Couldn't create socket");
        return 1;
    }
    if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error: Couldn't connect");
        return 1;
    }
    // Construct the HTTP GET request
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    // Send the request
    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Error: Couldn't send request");
        return 1;
    }
    // Receive and process response
    char response[BUFFER_SIZE];
    ssize_t bytes_received;
    char *header_end = NULL;
    while ((bytes_received = recv(sockfd, response, BUFFER_SIZE - 1, 0)) > 0) {
        response[bytes_received] = '\0';
        // Look for the end of the HTTP header
        if ((header_end = strstr(response, "\r\n\r\n")) != NULL) {
            header_end += 4; // Move past the header delimiter
            break;
        }
    }
    if (header_end == NULL) {
        perror("Error: Invalid response");
        close(sockfd);
        return 1;
    }
    text *result_text;
    result_text = cstring_to_text(header_end);
    PG_RETURN_TEXT_P(result_text);

}
