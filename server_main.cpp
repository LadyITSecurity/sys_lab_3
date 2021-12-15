//
// Created by linux on 15.12.2021.
//

#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include "check.hpp"
#include "common.h"
class game_server {
    int _listening_socket;

    void game(int sock_fd) {
        const int value = rand() % 10+1;
        message msg;
        msg.action = actions::START;
        std::cout << "Starting a new game with a value " << value << std::endl;
        if (!try_send(sock_fd, msg))
            return;

        int guess = ~value;
        while (guess != value) {
            message guess_msg;
            if (!try_recv(sock_fd, guess_msg))
                return;

            if (guess_msg.action != actions::GUESS) {
                throw std::logic_error("Invalid action value");
            }

            guess = guess_msg.data;
            std::cout << "Got a new guess " << guess << std::endl;
            message answer;
            answer.action = actions::ANSWER;
            answer.data = guess == value;
            if (!try_send(sock_fd, answer))
                return;
        }

        if (!try_recv(sock_fd, msg))
            return;

        if (msg.action == actions::CONTINUE)
            game(sock_fd);
        else if (msg.action == actions::END)
            return;
        throw std::logic_error("Invalid action value");
    }

public:

    game_server(unsigned short port) {
        _listening_socket = check(socket(AF_INET, SOCK_STREAM, 0));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons( port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        check(bind(_listening_socket, (sockaddr*)&addr, sizeof(addr)));
    }

    void start() {
        check(listen(_listening_socket, 1));
        sockaddr_in addr;
        socklen_t len;
        while (true) {
            int sock_fd = check(accept(_listening_socket, (sockaddr*)&addr, &len));
            std::cout << "Connected from " << inet_ntoa(addr.sin_addr) << std::endl;
            game(sock_fd);
            close(sock_fd);
            std::cout << "Disconnected from " << inet_ntoa(addr.sin_addr) << std::endl;
        }
    }
};

int main() {
    game_server gs(PORT);
    gs.start();
}
