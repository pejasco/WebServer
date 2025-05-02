}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return -1;
    }

    // Set the socket to non-blocking mode
    setNonBlockingMode(server_fd);

    // Bind the socket to a port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    // Start listening for connections
    if (listen(server_fd, 10) == -1) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    std::cout << "Server is running on port 8080..." << std::endl;

    // Accept connections in a loop
    while (true) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No client is connecting right now, continue the loop
                std::cout << "No connection yet, waiting..." << std::endl;
                usleep(100000); // Sleep for 100ms to avoid busy-waiting
                continue;
            } else {
                // An actual error occurred
                perror("accept");
                break;
            }
        }

        // Handle the new client connection
        std::cout << "New client connected!" << std::endl;

        // Close the client socket after handling (for simplicity)
        close(client_fd);
    }

    close(server_fd);
    return 0;
}