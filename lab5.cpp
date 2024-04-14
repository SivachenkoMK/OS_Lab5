#include <iostream>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


int f(int x) {
    return x % 2 == 0 ? 1 : 0;
}

int g(int x) {
    if (x % 2 != 0) {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    return x % 3 == 0 ? 1 : 0;
}

void processResults(int sock) {
    int buffer;
    while (true) {
        if (recv(sock, &buffer, sizeof(buffer), 0) > 0) {
            if (buffer == 0) {
                std::cout << "Результат: false" << std::endl;
                break;
            } else {
                std::cout << "Результат: true" << std::endl;
                break;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::cout << "Функція не повернула результат. Виберіть дію: 1) Продовжити 2) Припинити 3) Продовжити без запитань" << std::endl;
            int choice;
            std::cin >> choice;
            if (choice == 2) {
                break;
            } else if (choice == 3) {
                // Ignore
            }
        }
    }
}

int main() {
    int sockets[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

    std::thread fThread([&] {
        int result = f(10);
        send(sockets[0], &result, sizeof(result), 0);
    });

    std::thread gThread([&] {
        int result = g(10);
        send(sockets[1], &result, sizeof(result), 0);
    });

    std::thread processThread(processResults, sockets[1]);

    fThread.join();
    gThread.join();
    processThread.detach();

    return 0;
}
