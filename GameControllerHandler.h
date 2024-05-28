#pragma once

#include <SDL.h>
#include <iostream>

#include <Modelec/TCPClient.h>
#include <Modelec/Utils.h>

class GameControllerHandler : public TCPClient {
public:
    explicit GameControllerHandler(const char* ip = "127.0.0.1", int port = 8080);

    bool init();

    void start() override;

    void handleMessage(const std::string &message) override;

    void handleEvents();

    void closeController();

    void close();

    ~GameControllerHandler() override;

private:
    SDL_GameController* controller;

    double lidarDectectionDistance = 400;

    bool rumble = true;
};
