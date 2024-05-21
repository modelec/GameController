#include "GameControllerHandler.h"

#include <Modelec/CLParser.h>

#include <atomic>
#include <mutex>
#include <csignal>

std::atomic<bool> shouldStop = false;

void signalHandler( int signum ) {
    shouldStop = true;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);

    CLParser clParser(argc, argv);

    int port = clParser.getOption<int>("port", 8080);

    std::string host = clParser.getOption("host", "127.0.0.1");

    GameControllerHandler gameControllerHandler(host.c_str(), port);

    if (!gameControllerHandler.init()) {
        return 1;
    }

    gameControllerHandler.start();

    while (!shouldStop && !gameControllerHandler.shouldStop()) {
        usleep(500'000);
    }

    gameControllerHandler.stop();

    return 0;
}
