#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define CHUNK_MAX_SIZE 1000
#define PORT "80"

void printUsage(char *programName) {
    fprintf(stderr, "Usage: %s <chunkSize>\n", programName);
}

ssize_t sendAll(int sockfd, const char *buffer, size_t length) {
    size_t totalSent = 0;
    ssize_t sent;
    while (totalSent < length) {
        sent = send(sockfd, buffer + totalSent, length - totalSent, 0);
        if (sent == -1) {
            return -1;
        }
        totalSent += sent;
    }
    return totalSent;
}

ssize_t recvAll(int sockfd, char *buffer, size_t length) {
    size_t totalReceived = 0;
    ssize_t received;
    while (totalReceived < length) {
        received = recv(sockfd, buffer + totalReceived, length - totalReceived, 0);
        if (received == -1) {
            return -1;
        } else if (received == 0) {
            break;
        }
        totalReceived += received;
    }
    return totalReceived;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    int chunkSize = atoi(argv[1]);
    if (chunkSize <= 0 || chunkSize > CHUNK_MAX_SIZE) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("www.ecst.csuchico.edu", PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return EXIT_FAILURE;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    freeaddrinfo(res);
    const char *request = "GET /~kkredo/file.html HTTP/1.0\r\n\r\n";
    if (sendAll(sockfd, request, strlen(request)) == -1) {
        perror("send");
        close(sockfd);
        return EXIT_FAILURE;
    }

    char buffer[CHUNK_MAX_SIZE + 1];
    int h1Count = 0;
    int totalBytes = 0;

    while (1) {
        ssize_t bytesReceived = recvAll(sockfd, buffer, chunkSize);
        if (bytesReceived == -1) {
            perror("recv");
            close(sockfd);
            return EXIT_FAILURE;
        } else if (bytesReceived == 0) {
            break;
        }
        totalBytes += bytesReceived;
        buffer[bytesReceived] = '\0';

        char *ptr = buffer;
        while ((ptr = strstr(ptr, "<h1>")) != NULL) {
            h1Count++;
            ptr += 4;
        }
    }
    printf("Number of <h1> tags: %d\n", h1Count);
    printf("Number of bytes: %d\n", totalBytes);

    close(sockfd);
    return EXIT_SUCCESS;
}
