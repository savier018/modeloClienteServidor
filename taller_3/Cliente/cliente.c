#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    // Create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address struct
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP address
    server_address.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    // Send carrera y codigo to the server
    char input[MAX_BUFFER_SIZE];
    printf("Enter career and code (separated by space): ");
    fgets(input, sizeof(input), stdin);

    // Remove newline character from input
    input[strcspn(input, "\n")] = '\0';

    char carrera[MAX_BUFFER_SIZE];
    char codigo[MAX_BUFFER_SIZE];
    if (sscanf(input, "%s %s", carrera, codigo) != 2) {
        fprintf(stderr, "Error: formato de entrada incorrecto\n");
        return EXIT_FAILURE;
    }

    // Send data to the server
    write(client_socket, input, strlen(input));

    // Receive and print the server's response
    char buffer[MAX_BUFFER_SIZE];
    ssize_t read_size = read(client_socket, buffer, sizeof(buffer));
    if (read_size > 0) {
        buffer[read_size] = '\0';
        printf("Server response: %s\n", buffer);
    } else {
        perror("Error receiving data from server");
    }

    close(client_socket);

    return 0;
}
