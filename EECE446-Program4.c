#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>

#define MAX_PEERS 5
#define MAX_FILES 10
#define MAX_FILENAME_LEN 101
#define BUFFER_SIZE 1024

#pragma pack(push, 1)
struct SearchResponse {
    uint32_t id;
    uint32_t ip;
    uint16_t port;
};
#pragma pack(pop)

struct PeerEntry {
    uint32_t id;
    int socketDescriptor;
    char files[MAX_FILES][MAX_FILENAME_LEN];
    int fileCount;
    struct sockaddr_in address;
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int portNum = atoi(argv[1]);
    if (portNum <= 0) {
        fprintf(stderr, "Invalid port number: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    int listenSock;
    if ((listenSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int reuseOpt = 1;
    if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &reuseOpt, sizeof(reuseOpt)) < 0) {
        perror("setsockopt");
        close(listenSock);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portNum);
    if (bind(listenSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        close(listenSock);
        exit(EXIT_FAILURE);
    }
    if (listen(listenSock, MAX_PEERS) < 0) {
        perror("listen");
        close(listenSock);
        exit(EXIT_FAILURE);
    }
    struct PeerEntry peers[MAX_PEERS];
    for (int i = 0; i < MAX_PEERS; i++) {
        peers[i].socketDescriptor = -1;
        peers[i].fileCount = 0;
        peers[i].id = 0;
    }
    while (1) {
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(listenSock, &readFds);
        int maxFdVal = listenSock;
        for (int i = 0; i < MAX_PEERS; i++) {
            if (peers[i].socketDescriptor != -1) {
                FD_SET(peers[i].socketDescriptor, &readFds);
                if (peers[i].socketDescriptor > maxFdVal)
                    maxFdVal = peers[i].socketDescriptor;
            }
        }
        int activity = select(maxFdVal + 1, &readFds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select");
            continue;
        }
        if (FD_ISSET(listenSock, &readFds)) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int newSock = accept(listenSock, (struct sockaddr *)&clientAddr, &clientLen);
            if (newSock < 0) {
                perror("accept");
            } else {
                int added = 0;
                for (int i = 0; i < MAX_PEERS; i++) {
                    if (peers[i].socketDescriptor == -1) {
                        peers[i].socketDescriptor = newSock;
                        peers[i].address = clientAddr;
                        peers[i].fileCount = 0;
                        peers[i].id = 0;
                        added = 1;
                        break;
                    }
                }
                if (!added) {
                    close(newSock);
                }
            }
        }
        char recvBuffer[BUFFER_SIZE];
        for (int i = 0; i < MAX_PEERS; i++) {
            int sock = peers[i].socketDescriptor;
            if (sock != -1 && FD_ISSET(sock, &readFds)) {
                int bytesRead = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
                if (bytesRead <= 0) {
                    close(sock);
                    peers[i].socketDescriptor = -1;
                    peers[i].fileCount = 0;
                    peers[i].id = 0;
                    continue;
                }
                if (bytesRead < 1)
                    continue;
                uint8_t actionId;
                memcpy(&actionId, recvBuffer, sizeof(actionId));
                switch (actionId) {
                    case 0: {
                        if (bytesRead < 1 + 4)
                            break;
                        socklen_t addrLen = sizeof(peers[i].address);
                        if (getpeername(sock, (struct sockaddr *)&peers[i].address, &addrLen) < 0) {
                            perror("getpeername");
                            close(sock);
                            peers[i].socketDescriptor = -1;
                            break;
                        }
                        uint32_t netId;
                        memcpy(&netId, recvBuffer + sizeof(actionId), sizeof(netId));
                        peers[i].id = ntohl(netId);
                        printf("TEST] JOIN %u\n", peers[i].id);
                        fflush(stdout);
                        break;
                    }
                    case 1: {
                        if (bytesRead < 1 + 4)
                            break;
                        uint32_t netCount;
                        memcpy(&netCount, recvBuffer + sizeof(actionId), sizeof(netCount));
                        int fileCount = ntohl(netCount);
                        if (fileCount > MAX_FILES)
                            fileCount = MAX_FILES;
                        peers[i].fileCount = fileCount;
                        int bufIndex = 1 + sizeof(uint32_t);
                        for (int j = 0; j < fileCount; j++) {
                            strncpy(peers[i].files[j], recvBuffer + bufIndex, MAX_FILENAME_LEN - 1);
                            peers[i].files[j][MAX_FILENAME_LEN - 1] = '\0';
                            bufIndex += strlen(recvBuffer + bufIndex) + 1;
                        }
                        printf("TEST] PUBLISH %d", fileCount);
                        for (int j = 0; j < fileCount; j++) {
                            printf(" %s", peers[i].files[j]);
                        }
                        printf("\n");
                        fflush(stdout);
                        break;
                    }
                    case 2: {
                        if (bytesRead < 1 + 1)
                            break;
                        char searchFile[MAX_FILENAME_LEN];
                        strncpy(searchFile, recvBuffer + sizeof(actionId), MAX_FILENAME_LEN - 1);
                        searchFile[MAX_FILENAME_LEN - 1] = '\0';
                        for (int k = 0; k < MAX_PEERS; k++) {
                            if (peers[k].socketDescriptor != -1) {
                                for (int j = 0; j < peers[k].fileCount; j++) {
                                    if (strcmp(peers[k].files[j], searchFile) == 0) {
                                        uint32_t peerId = peers[k].id;
                                        char ipStr[INET_ADDRSTRLEN];
                                        inet_ntop(AF_INET, &peers[k].address.sin_addr, ipStr, INET_ADDRSTRLEN);
                                        int portNumLocal = ntohs(peers[k].address.sin_port);
                                        printf("TEST] SEARCH %s %u %s:%d\n", searchFile, peerId, ipStr, portNumLocal);
                                        fflush(stdout);
                                        char dataPacket[10];
                                        uint32_t idNet = htonl(peerId);
                                        uint16_t portNet = peers[k].address.sin_port;
                                        memcpy(dataPacket, &idNet, 4);
                                        memcpy(dataPacket + 4, &peers[k].address.sin_addr.s_addr, 4);
                                        memcpy(dataPacket + 8, &portNet, 2);
                                        if (send(sock, dataPacket, 10, 0) < 0) {
                                            perror("send");
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }
    close(listenSock);
    return 0;
}
