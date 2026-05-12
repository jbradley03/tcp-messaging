#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>

void receive_messages(int sock) {

    char buffer[1024];

    while (true) {

        ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);  // read bytes from socket into "buffer"

        if (bytes <= 0) {
            std::cout << "Disconnected from server\n";
            break;
        }

        buffer[bytes] = '\0';   // null terminate buffer

        std::cout << "\nMessage: " << buffer << std::endl;
    }
}

int main() {                // client connects and has two threads, one waits to receive one waits for send

    int sock = socket(AF_INET, SOCK_STREAM, 0);     

    sockaddr_in server_addr{};

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9090);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));

    std::thread receiver(receive_messages, sock);  // initialise thread receive_messages and pass socket

    while (true) {

        std::string message;

        std::getline(std::cin, message);

        send(sock, message.c_str(), message.size(), 0);
    }

    receiver.join();

    close(sock);

    return 0;
}