#include <iostream>

#include "tcp.h"

int main() {
    try {
        Socket tcp = Socket(SOCK_STREAM, INADDR_ANY, 3000);
        tcp.initilize();
    } catch (std::runtime_error &e) {
        std::cout << e.what() << ": ";
        fflush(stdout);
        perror(nullptr);
        return 1;
    }

    getchar();
}
