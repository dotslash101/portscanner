#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <getopt.h>

#define TIMEOUT 1  // Connection timeout in seconds

void scan(const char *address, int port);
void parse_ports(const char *port_range, const char *address);

void parse_ports(const char *port_range, const char *address) {
    char *port_range_copy = strdup(port_range);
    char *token = strtok(port_range_copy, ",");
    while (token != NULL) {
        if (strstr(token, "-")) {
            int start, end;
            sscanf(token, "%d-%d", &start, &end);
            for (int port = start; port <= end; port++) {
                scan(address, port);
            }
        } else {
            int port = atoi(token);
            scan(address, port);
        }
        token = strtok(NULL, ",");
    }
    free(port_range_copy);
}

void scan(const char *address, int port) {
    int sockfd;
    struct sockaddr_in target;

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return;
    }

    // Set the target address
    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, address, &target.sin_addr);

    // Set socket timeout
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

    // Attempt to connect to the port
    if (connect(sockfd, (struct sockaddr *)&target, sizeof(target)) == 0) {
        printf("[OPEN] %d/TCP\n", port);
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    const char *address = NULL;
    const char *ports = NULL;

    int opt;
    struct option long_options[] = {
        {"address", required_argument, NULL, 'a'},
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "a:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'a':
                address = optarg;
                break;
            case 'p':
                ports = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s -a <address> -p <port_range>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (!address || !ports) {
        fprintf(stderr, "Both address and port range are required.\n");
        exit(EXIT_FAILURE);
    }

    parse_ports(ports, address);
    return 0;
}
