#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_BUFFER 1200
#define MAX_FILENAME 100
#define SHARED_DIR "SharedFiles"

void send_join(int sockfd, uint32_t peer_id);
void send_publish(int sockfd);
void send_search(int sockfd);
void handle_search_response(int sockfd);

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <registry_host> <registry_port> <peer_id>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *registry_host = argv[1];
    int registry_port = atoi(argv[2]);
    uint32_t peer_id = strtoul(argv[3], NULL, 10);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct hostent *server = gethostbyname(registry_host);
    if (server == NULL) {
        fprintf(stderr, "Error: No such host\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in registry_addr;
    memset(&registry_addr, 0, sizeof(registry_addr));
    registry_addr.sin_family = AF_INET;
    memcpy(&registry_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    registry_addr.sin_port = htons(registry_port);

    if (connect(sockfd, (struct sockaddr *)&registry_addr, sizeof(registry_addr)) < 0) {
        perror("Connection to registry failed");
        exit(EXIT_FAILURE);
    }

    char command[32];

    while (1) {
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "JOIN") == 0) {
            send_join(sockfd, peer_id);
        } else if (strcmp(command, "PUBLISH") == 0) {
            send_publish(sockfd);
        } else if (strcmp(command, "SEARCH") == 0) {
            send_search(sockfd);
        } else if (strcmp(command, "EXIT") == 0) {
            close(sockfd);
            break;
        } else {
            printf("Invalid command\n");
        }
    }
    return 0;
}

void send_join(int sockfd, uint32_t peer_id) {
    char buffer[5];
    buffer[0] = 0;
    uint32_t network_peer_id = htonl(peer_id);
    memcpy(buffer + 1, &network_peer_id, sizeof(network_peer_id));
    if (send(sockfd, buffer, sizeof(buffer), 0) < 0) {
        perror("JOIN request failed");
    }
}

void send_publish(int sockfd) {
    DIR *dir = opendir(SHARED_DIR);
    if (dir == NULL) {
        perror("Failed to open shared directory");
        return;
    }

    struct dirent *entry;
    char buffer[MAX_BUFFER];
    int offset = 5;
    int file_count = 0;
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
            file_count++;
        }
    }

    closedir(dir);
    uint32_t network_file_count = htonl(file_count);
    memcpy(buffer + 1, &network_file_count, sizeof(network_file_count));

    if (send(sockfd, buffer, offset, 0) < 0) {
        perror("PUBLISH request failed");
    }
}

void send_search(int sockfd) {
    char file_name[MAX_FILENAME];

    printf("Enter a file name: ");
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0';

    char buffer[MAX_FILENAME + 1];
    buffer[0] = 2;
    strcpy(buffer + 1, file_name);

    if (send(sockfd, buffer, strlen(file_name) + 2, 0) < 0) {
        perror("SEARCH request failed");
        return;
    }
    handle_search_response(sockfd);
}

void handle_search_response(int sockfd) {
    char buffer[10];

    if (recv(sockfd, buffer, sizeof(buffer), 0) <= 0) {
        perror("SEARCH response failed");
        return;
    }

    uint32_t peer_id, peer_ip, peer_port;
    memcpy(&peer_id, buffer, 4);
    memcpy(&peer_ip, buffer + 4, 4);
    memcpy(&peer_port, buffer + 8, 2);

    peer_id = ntohl(peer_id);
    peer_ip = ntohl(peer_ip);
    peer_port = ntohs(peer_port);

    if (peer_id == 0 && peer_ip == 0 && peer_port == 0) {
        printf("File not indexed by registry\n");
    } else {
        unsigned char bytes[4];
        bytes[0] = (peer_ip >> 24) & 0xFF;
        bytes[1] = (peer_ip >> 16) & 0xFF;
        bytes[2] = (peer_ip >> 8) & 0xFF;
        bytes[3] = peer_ip & 0xFF;

        printf("File found at Peer %u %u.%u.%u.%u:%u\n", peer_id, bytes[0], bytes[1], bytes[2], bytes[3], peer_port);
    }
}
