#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include "check.hpp"
#include "common.h"

class game_client {
    int _socket;

    void game() {
        message msg;
        if (!try_recv(_socket, msg))
            return;

        if(msg.action != actions::START)
            throw std::logic_error("Invalid action value");
        bool ok = false;
        while (!ok) {
            int guess;
            std::cout << "Make a guess: ";
            do {
                std::cin >> guess;
            } while (std::cin.fail());

            message guess_msg;
            guess_msg.action = actions::GUESS;
            guess_msg.data = guess;
            if (!try_send(_socket, guess_msg))
                return;

            message answer_msg;
            if (!try_recv(_socket, answer_msg))
                return;

            if (answer_msg.action != actions::ANSWER)
                throw std::logic_error("Invalid action value");
            ok = answer_msg.data;
            if (!ok)
                std::cout << "Wrong" << std::endl;
        }
        std::cout << "Right" << std::endl;

        std::cout << "Again?";
        std::string s;
        std::cin >> s;


        if (s != "yes") {
            msg.action = actions::END;
            send(_socket, &msg, sizeof(msg), 0);
            return;
        }
        msg.action = actions::CONTINUE;
        if (!try_send(_socket, msg))
            return;
        game();
    }

public:
    void start(int port) {
        _socket = check(socket(AF_INET, SOCK_STREAM, 0));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        check(connect(_socket, (sockaddr*)&addr, sizeof(addr)));
        game();
    }
};


int main() {
    game_client g;
    g.start(PORT);
}