#include "headers.h"

#define MAX_BUFFER_SIZE 4096

void iman(char *cmd) {

    const char *page = strtok(cmd, "\r\t\n "); // trimmed
    const char *host = "man.he.net";
    const char *port = "80"; // HTTP port
                             // http://man.he.net/?topic=sleep&section=all

    // from https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    struct addrinfo hints;
    struct addrinfo *servinfo; // will point to a linked list

    memset(&hints, 0, sizeof(hints)); // making sure the structs are empty
    hints.ai_family = AF_UNSPEC;      // IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &servinfo) != 0) {
        perror("Issues getting address info\n");
        return;
    }

    int sock_temp = -1;
    struct addrinfo *ai_curr;
    for (ai_curr = servinfo; ai_curr != NULL;
         ai_curr = ai_curr->ai_next) { // traversing the linked list
        sock_temp = socket(ai_curr->ai_family, ai_curr->ai_socktype,
                           ai_curr->ai_protocol);
        if (sock_temp != -1) {
            if (connect(sock_temp, ai_curr->ai_addr, ai_curr->ai_addrlen) ==
                -1) {
                perror("Issues connecting to the socket\n");
                return;
            }
            break; // connected
        }
    }

    char *req;
    asprintf(&req, "GET /?topic=%s&section=all HTTP/1.1\r\nHost: %s\r\n\r\n",
             page, host);
    if (send(sock_temp, req, strlen(req), 0) == -1) {
        perror("Issues sending request! Check your internet!\n");
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(sock_temp, buffer, sizeof(buffer), 0)) > 0) {
        if (strstr(buffer, "No matches") != NULL) {
            printf("Page does not exist.\n");
            break;
        }

        char *find_beg = strstr(buffer, "NAME\n"); // starts with this
        char *find_end = strstr(buffer, "AUTHOR"); // everything before AUTHOR

        if (find_beg == NULL)
            find_beg = buffer;

        if (find_end == NULL) {
            printf("%s", find_beg);
        } else {
            int length = find_end - find_beg;
            find_beg[length - 1] = '\0';
            printf("%s", find_beg);
        }
    }

    close(sock_temp);
    freeaddrinfo(servinfo);
    free(req);

    return;
}
