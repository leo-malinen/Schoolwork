#peer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_BUFFER 1200
#define MAX_FILENAME 100
#define CHUNK_SIZE 100
#define SHARED_DIR "SharedFiles"

void sendJoin(int sockFd, uint32_t peerId);
void sendPublish(int sockFd);
void sendSearch(int sockFd);
void sendFetch(int sockFd);
void handleSearchResponse(int sockFd);
int recvall(int sockFd, char *buf);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <registry_host> <registry_port> <peer_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *registryHost = argv[1];
    int registryPort = atoi(argv[2]);
    uint32_t peerId = strtoul(argv[3], NULL, 10);
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    struct hostent *server = gethostbyname(registryHost);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in registryAddr;
    memset(&registryAddr, 0, sizeof(registryAddr));
    registryAddr.sin_family = AF_INET;
    memcpy(&registryAddr.sin_addr.s_addr, server->h_addr, server->h_length);
    registryAddr.sin_port = htons(registryPort);
    if (connect(sockFd, (struct sockaddr *)&registryAddr, sizeof(registryAddr)) < 0) {
        perror("Connection to registry failed");
        exit(EXIT_FAILURE);
    }
    char command[32];
    while (1) {
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';
        if (strcmp(command, "JOIN") == 0) {
            sendJoin(sockFd, peerId);
        } else if (strcmp(command, "PUBLISH") == 0) {
            sendPublish(sockFd);
        } else if (strcmp(command, "SEARCH") == 0) {
            sendSearch(sockFd);
        } else if (strcmp(command, "FETCH") == 0) {
            sendFetch(sockFd);
        } else if (strcmp(command, "EXIT") == 0) {
            close(sockFd);
            break;
        } else {
            printf("Invalid command\n");
        }
    }
    return 0;
}

void sendJoin(int sockFd, uint32_t peerId) {
    char buffer[5];
    buffer[0] = 0;
    uint32_t networkPeerId = htonl(peerId);
    memcpy(buffer + 1, &networkPeerId, sizeof(networkPeerId));
    if (send(sockFd, buffer, sizeof(buffer), 0) < 0) {
        perror("JOIN request failed");
    }
}

void sendPublish(int sockFd) {
    DIR *dir = opendir(SHARED_DIR);
    if (dir == NULL) {
        perror("Failed to open shared directory");
        return;
    }
    struct dirent *entry;
    char buffer[MAX_BUFFER];
    int offset = 5;
    int fileCount = 0;
    buffer[0] = 1;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            size_t len = strlen(entry->d_name) + 1;
            if (offset + len > MAX_BUFFER) {
                fprintf(stderr, "PUBLISH request too large\n");
                closedir(dir);
                return;
            }
            strcpy(buffer + offset, entry->d_name);
            offset += len;
            fileCount++;
        }
    }
    closedir(dir);
    uint32_t networkFileCount = htonl(fileCount);
    memcpy(buffer + 1, &networkFileCount, sizeof(networkFileCount));
    if (send(sockFd, buffer, offset, 0) < 0) {
        perror("PUBLISH request failed");
    }
}

void sendSearch(int sockFd) {
    char fileName[MAX_FILENAME];
    printf("Enter a file name: ");
    fgets(fileName, sizeof(fileName), stdin);
    fileName[strcspn(fileName, "\n")] = '\0';
    char buffer[MAX_FILENAME + 1];
    buffer[0] = 2;
    strcpy(buffer + 1, fileName);
    if (send(sockFd, buffer, strlen(fileName) + 2, 0) < 0) {
        perror("SEARCH request failed");
        return;
    }
    handleSearchResponse(sockFd);
}

void sendFetch(int sockFd) {
    char fileName[MAX_FILENAME];
    printf("Enter a file name: ");
    fgets(fileName, sizeof(fileName), stdin);
    fileName[strcspn(fileName, "\n")] = '\0';
    char sendPacket[MAX_FILENAME + 1] = {0};
    sendPacket[0] = 2;
    strcpy(sendPacket + 1, fileName);
    if (send(sockFd, sendPacket, strlen(fileName) + 2, 0) < 0) {
        perror("SEARCH request for FETCH failed");
        return;
    }
    char response[10];
    if (recv(sockFd, response, sizeof(response), 0) <= 0) {
        perror("SEARCH response failed");
        return;
    }
    uint32_t peerId;
    struct in_addr addr;
    unsigned short peerPort;
    memcpy(&peerId, response, 4);
    memcpy(&addr.s_addr, response + 4, 4);
    memcpy(&peerPort, response + 8, 2);
    peerId = ntohl(peerId);
    const char *peerIp = inet_ntoa(addr);
    peerPort = ntohs(peerPort);
    if (peerId == 0) {
        printf("File not indexed by registry\n");
        return;
    }
    char newPeerPort[6];
    sprintf(newPeerPort, "%d", peerPort);
    int newSockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (newSockFd < 0) {
        perror("Socket creation failed");
        return;
    }
    struct sockaddr_in peerAddr;
    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    inet_pton(AF_INET, peerIp, &peerAddr.sin_addr);
    peerAddr.sin_port = htons(peerPort);
    if (connect(newSockFd, (struct sockaddr *)&peerAddr, sizeof(peerAddr)) < 0) {
        perror("Connection to peer failed");
        close(newSockFd);
        return;
    }
    char fetchPacket[MAX_FILENAME + 2] = {0};
    fetchPacket[0] = 3;
    strcpy(fetchPacket + 1, fileName);
    if (send(newSockFd, fetchPacket, strlen(fileName) + 2, 0) < 0) {
        perror("FETCH request failed");
        close(newSockFd);
        return;
    }
    char responseCode;
    if (recv(newSockFd, &responseCode, 1, 0) <= 0) {
        perror("Missing FETCH response code");
        close(newSockFd);
        return;
    }
    if (responseCode != 0) {
        printf("Invalid FETCH response\n");
        close(newSockFd);
        return;
    }
    FILE *file = fopen(fileName, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        close(newSockFd);
        return;
    }
    char recvBuffer[CHUNK_SIZE];
    int bytesReceived;
    int totalBytesReceived = 0;
    while ((bytesReceived = recvall(newSockFd, recvBuffer)) > 0) {
        fwrite(recvBuffer, 1, bytesReceived, file);
        totalBytesReceived += bytesReceived;
    }
    fclose(file);
    close(newSockFd);
    if (bytesReceived < 0)
        printf("Error occurred while receiving file\n");
    else
        printf("File successfully received and saved as '%s' (%d bytes)\n", fileName, totalBytesReceived);
}

int recvall(int sockFd, char *buf) {
    int totalReceived = 0, n;
    while (totalReceived < CHUNK_SIZE) {
        n = recv(sockFd, buf + totalReceived, CHUNK_SIZE - totalReceived, 0);
        if (n < 0)
            return -1;
        if (n == 0)
            break;
        totalReceived += n;
    }
    return totalReceived;
}

void handleSearchResponse(int sockFd) {
    char buffer[10];
    uint32_t peerId;
    struct in_addr addr;
    unsigned short peerPort;
    if (recv(sockFd, buffer, sizeof(buffer), 0) <= 0) {
        perror("SEARCH response failed");
        return;
    }
    memcpy(&peerId, buffer, 4);
    memcpy(&addr.s_addr, buffer + 4, 4);
    memcpy(&peerPort, buffer + 8, 2);
    peerId = ntohl(peerId);
    const char *peerIp = inet_ntoa(addr);
    peerPort = ntohs(peerPort);
    if (peerId != 0) {
        printf("File found at\n");
        printf("Peer %u\n", peerId);
        printf("%s:%u\n", peerIp, peerPort);
    } else {
        printf("File not indexed by registry\n");
    }
}

#program 3 file
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CMD 64
#define MAX_FILENAME 128
#define BUFFER_SIZE 1024
#define SEARCH_ACTION 2
#define FETCH_ACTION 3
#define JOIN_ACTION 1
#define REGISTER_ACTION 4
#define PUBLISH_ACTION 5

void trimNewline(char *str) {
    str[strcspn(str, "\n")] = '\0';
}

void handleJoin(const char *registryIp, int registryPort, int localPeerPort, int debug) {
    unsigned char msg[2];
    msg[0] = JOIN_ACTION;
    msg[1] = '\0';

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket (JOIN)");
        return;
    }
    struct sockaddr_in regAddr;
    regAddr.sin_family = AF_INET;
    regAddr.sin_port = htons(registryPort);
    inet_pton(AF_INET, registryIp, &regAddr.sin_addr);
    if (connect(sockFd, (struct sockaddr *)&regAddr, sizeof(regAddr)) < 0) {
        perror("connect (JOIN)");
        close(sockFd);
        return;
    }
    if (send(sockFd, msg, sizeof(msg), 0) != sizeof(msg)) {
        perror("send JOIN");
    } else if (debug) {
        printf("[DEBUG] JOIN message sent to registry %s:%d\n", registryIp, registryPort);
    }
    close(sockFd);
}

void handlePublish(const char *registryIp, int registryPort, int localPeerPort, int debug) {
    char fileName[] = "example.txt";
    size_t msgLen = 1 + strlen(fileName) + 1;
    unsigned char *msg = malloc(msgLen);
    if (!msg) {
        perror("malloc (PUBLISH)");
        return;
    }
    msg[0] = PUBLISH_ACTION;
    strcpy((char *)(msg + 1), fileName);

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket (PUBLISH)");
        free(msg);
        return;
    }
    struct sockaddr_in regAddr;
    regAddr.sin_family = AF_INET;
    regAddr.sin_port = htons(registryPort);
    inet_pton(AF_INET, registryIp, &regAddr.sin_addr);
    if (connect(sockFd, (struct sockaddr *)&regAddr, sizeof(regAddr)) < 0) {
        perror("connect (PUBLISH)");
        close(sockFd);
        free(msg);
        return;
    }
    if (send(sockFd, msg, msgLen, 0) != msgLen) {
        perror("send PUBLISH");
    } else if (debug) {
        printf("[DEBUG] PUBLISH message sent for file %s.\n", fileName);
    }
    free(msg);
    close(sockFd);
}

int handleSearchForFetch(const char *registryIp, int registryPort, const char *filename,
                         char *peerIp, int *peerPort, int debug) {
    size_t fileNameLen = strlen(filename);
    size_t msgLen = 1 + fileNameLen + 1;
    unsigned char *msg = malloc(msgLen);
    if (!msg) {
        perror("malloc (SEARCH)");
        return 0;
    }
    msg[0] = SEARCH_ACTION;
    strcpy((char *)(msg + 1), filename);

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket (SEARCH)");
        free(msg);
        return 0;
    }
    struct sockaddr_in regAddr;
    regAddr.sin_family = AF_INET;
    regAddr.sin_port = htons(registryPort);
    inet_pton(AF_INET, registryIp, &regAddr.sin_addr);
    if (connect(sockFd, (struct sockaddr *)&regAddr, sizeof(regAddr)) < 0) {
        perror("connect (SEARCH)");
        free(msg);
        close(sockFd);
        return 0;
    }
    if (send(sockFd, msg, msgLen, 0) != msgLen) {
        perror("send SEARCH");
        free(msg);
        close(sockFd);
        return 0;
    }
    free(msg);
    char buffer[256];
    ssize_t n = recv(sockFd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        perror("recv SEARCH");
        close(sockFd);
        return 0;
    }
    buffer[n] = '\0';
    close(sockFd);
    if (strncmp(buffer, "NOT FOUND", 9) == 0) {
        if (debug)
            printf("[DEBUG] SEARCH: File %s not found in registry.\n", filename);
        return 0;
    }
    char ipStr[INET_ADDRSTRLEN];
    int port;
    if (sscanf(buffer, "%15[^:]:%d", ipStr, &port) == 2) {
        strcpy(peerIp, ipStr);
        *peerPort = port;
        if (debug)
            printf("[DEBUG] SEARCH result: %s:%d for file %s\n", peerIp, *peerPort, filename);
        return 1;
    }
    return 0;
}

void handleSearch(const char *registryIp, int registryPort, const char *filename, int debug) {
    char targetIp[INET_ADDRSTRLEN];
    int targetPort = 0;
    if (handleSearchForFetch(registryIp, registryPort, filename, targetIp, &targetPort, debug))
        printf("File '%s' found at peer %s:%d\n", filename, targetIp, targetPort);
    else
        printf("File '%s' not found in registry.\n", filename);
}

void handleFetch(const char *filename, const char *peerIp, int peerPort, int debug) {
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket (FETCH)");
        return;
    }
    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);
    if (inet_pton(AF_INET, peerIp, &peerAddr.sin_addr) <= 0) {
        perror("inet_pton (FETCH)");
        close(sockFd);
        return;
    }
    if (connect(sockFd, (struct sockaddr *)&peerAddr, sizeof(peerAddr)) < 0) {
        perror("connect (FETCH)");
        close(sockFd);
        return;
    }
    size_t fileNameLen = strlen(filename);
    size_t msgLen = 1 + fileNameLen + 1;
    unsigned char *msg = malloc(msgLen);
    if (!msg) {
        perror("malloc (FETCH)");
        close(sockFd);
        return;
    }
    msg[0] = FETCH_ACTION;
    strcpy((char *)(msg + 1), filename);
    if (send(sockFd, msg, msgLen, 0) != msgLen) {
        perror("send FETCH");
        free(msg);
        close(sockFd);
        return;
    }
    free(msg);
    if (debug) {
        printf("[DEBUG] FETCH request sent to peer %s:%d for file '%s'\n", peerIp, peerPort, filename);
    }
    unsigned char responseCode;
    ssize_t recvResult = recv(sockFd, &responseCode, 1, 0);
    if (recvResult <= 0) {
        perror("recv FETCH response code");
        close(sockFd);
        return;
    }
    if (responseCode != 0) {
        printf("Error fetching file. Response code: %d\n", responseCode);
        close(sockFd);
        return;
    }
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen (FETCH writing file)");
        close(sockFd);
        return;
    }
    char data[BUFFER_SIZE];
    ssize_t bytesReceived;
    while ((bytesReceived = recv(sockFd, data, sizeof(data), 0)) > 0) {
        if (fwrite(data, 1, bytesReceived, fp) != (size_t)bytesReceived) {
            perror("fwrite");
            break;
        }
    }
    if (bytesReceived < 0)
        perror("recv file data (FETCH)");
    fclose(fp);
    close(sockFd);
    printf("File '%s' successfully downloaded.\n", filename);
}

void handleRegister(const char *registryIp, int registryPort, int localPeerPort, int debug) {
    unsigned char msg[1 + 4 + 4 + 2];
    msg[0] = REGISTER_ACTION;
    uint32_t peerId = htonl(0x12345678);
    memcpy(msg + 1, &peerId, 4);
    struct in_addr addr;
    if (inet_pton(AF_INET, "127.0.0.1", &addr) <= 0) {
        perror("inet_pton (REGISTER)");
        return;
    }
    memcpy(msg + 1 + 4, &addr.s_addr, 4);
    uint16_t netPort = htons(localPeerPort);
    memcpy(msg + 1 + 4 + 4, &netPort, 2);

    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("socket (REGISTER)");
        return;
    }
    struct sockaddr_in regAddr;
    regAddr.sin_family = AF_INET;
    regAddr.sin_port = htons(registryPort);
    inet_pton(AF_INET, registryIp, &regAddr.sin_addr);
    if (connect(sockFd, (struct sockaddr *)&regAddr, sizeof(regAddr)) < 0) {
        perror("connect (REGISTER)");
        close(sockFd);
        return;
    }
    if (send(sockFd, msg, sizeof(msg), 0) != sizeof(msg)) {
        perror("send REGISTER");
    } else if (debug) {
        printf("[DEBUG] REGISTER message sent to registry %s:%d\n", registryIp, registryPort);
    }
    close(sockFd);
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <local_peer_port> <registry_ip> <registry_port> [-d] [-t]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int localPeerPort = atoi(argv[1]);
    char *registryIp = argv[2];
    int registryPort = atoi(argv[3]);

    int debug = 0;
    for (int i = 4; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0)
            debug = 1;
        else if (strcmp(argv[i], "-t") == 0 && debug)
            printf("[DEBUG] Test mode enabled.\n");
    }

    char cmd[MAX_CMD];
    char filename[MAX_FILENAME];
    char targetIp[INET_ADDRSTRLEN];
    int targetPort;

    printf("P2P Peer Application started.\n");
    printf("Commands available: JOIN, PUBLISH, SEARCH, FETCH, REGISTER, EXIT\n");

    while (1) {
        printf("Enter command: ");
        if (!fgets(cmd, sizeof(cmd), stdin))
            break;
        trimNewline(cmd);

        if (strcasecmp(cmd, "JOIN") == 0)
            handleJoin(registryIp, registryPort, localPeerPort, debug);
        else if (strcasecmp(cmd, "PUBLISH") == 0)
            handlePublish(registryIp, registryPort, localPeerPort, debug);
        else if (strcasecmp(cmd, "SEARCH") == 0) {
            printf("Enter filename: ");
            if (!fgets(filename, sizeof(filename), stdin))
                continue;
            trimNewline(filename);
            handleSearch(registryIp, registryPort, filename, debug);
        }
        else if (strcasecmp(cmd, "FETCH") == 0) {
            printf("Enter filename: ");
            if (!fgets(filename, sizeof(filename), stdin))
                continue;
            trimNewline(filename);
            if (handleSearchForFetch(registryIp, registryPort, filename, targetIp, &targetPort, debug))
                handleFetch(filename, targetIp, targetPort, debug);
            else
                printf("File '%s' not found in registry.\n", filename);
        }
        else if (strcasecmp(cmd, "REGISTER") == 0)
            handleRegister(registryIp, registryPort, localPeerPort, debug);
        else if (strcasecmp(cmd, "EXIT") == 0) {
            printf("Exiting peer application.\n");
            break;
        }
        else {
            printf("Unknown command: %s\n", cmd);
        }
    }
    return 0;
}
