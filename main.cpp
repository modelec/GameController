#include "GameControllerHandler.h"

#include <atomic>
#include <mutex>
#include <csignal>

std::atomic<bool> shouldStop = false;

void signalHandler( int signum ) {
    shouldStop = true;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    int port = 8080;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    GameControllerHandler gameControllerHandler("127.0.0.1", port);

    if (!gameControllerHandler.init()) {
        return 1;
    }

    gameControllerHandler.start();

    gameControllerHandler.sendMessage("gc;strat;ready;1\n");

    while (!shouldStop && !gameControllerHandler.shouldStop()) {
        usleep(500'000);
    }

    gameControllerHandler.stop();

    return 0;
}
