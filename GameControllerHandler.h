#pragma once

#include <SDL.h>
#include <iostream>

#include <TCPSocket/TCPClient.hpp>

#include "utils.h"

class GameControllerHandler : public TCPClient {
public:
    explicit GameControllerHandler(const char* ip = "127.0.0.1", int port = 8080) : TCPClient(ip, port), controller(nullptr) {}

    bool init() {
        if (SDL_Init(SDL_INIT_GAMECONTROLLER) != 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        if (!SDL_IsGameController(0)) {
            std::cerr << "No compatible controllers connected.\n";
            return false;
        }
        controller = SDL_GameControllerOpen(0);
        if (controller == nullptr) {
            std::cerr << "Failed to open game controller: " << SDL_GetError() << std::endl;
            return false;
        }
        return true;
    }

    void start() {
        TCPClient::start();

        std::thread t(&GameControllerHandler::handleEvents, this);
        t.detach();
    }

    void handleMessage(const std::string &message) override {
        std::vector<std::string> tokens = Utils::split(message, ";");

        if (tokens[1] == "all" || tokens[1] == "gc") {
            if (tokens[2] == "stop proximity") {
                if (SDL_GameControllerRumble(controller, 0xFFFF, 0xFFFF, 1000) != 0) {
                    std::cerr << "Erreur lors de l'activation de la vibration : " << SDL_GetError() << std::endl;
                }
            }

            if (tokens[2] == "ready") {
                this->sendMessage("gc;all;game mode;gc\n");
            }
        }
    }

    void handleEvents() {
        while (running) {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        running = false;
                    break;
                    case SDL_CONTROLLERAXISMOTION:
                        if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
                            // std::cout << "Trigger left moved to " << event.caxis.value << std::endl;
                            this->sendMessage("gc;strat;trigger;0," + std::to_string(event.caxis.value) + "\n");
                        } else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
                            // std::cout << "Trigger right moved to " << event.caxis.value << std::endl;
                            this->sendMessage("gc;strat;trigger;1," + std::to_string(event.caxis.value) + "\n");
                        }
                        else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                            // std::cout << "Left X axis moved to " << event.caxis.value << std::endl;
                            this->sendMessage("gc;strat;axis;0," + std::to_string(event.caxis.value) + "\n");
                        }
                        else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                            // std::cout << "Left Y axis moved to " << event.caxis.value << std::endl;
                            this->sendMessage("gc;strat;axis;1," + std::to_string(event.caxis.value) + "\n");
                        }
                        else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                            // std::cout << "Right X axis moved to " << event.caxis.value << std::endl;
                            this->sendMessage("gc;strat;axis;2," + std::to_string(event.caxis.value) + "\n");
                        }
                        else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
                            // std::cout << "Right Y axis moved to " << event.caxis.value << std::endl;
                            // this->sendMessage("gc;strat;axis;3," + std::to_string(event.caxis.value) + "\n");
                        }
                    break;
                    case SDL_CONTROLLERBUTTONDOWN:
                        // std::cout << "Button down " << static_cast<int>(event.cbutton.button) << std::endl;
                        this->sendMessage("gc;strat;button down;" + std::to_string(event.cbutton.button) + "\n");
                    break;
                    case SDL_CONTROLLERBUTTONUP:
                        // std::cout << "Button up " << static_cast<int>(event.cbutton.button) << std::endl;
                        this->sendMessage("gc;strat;button up;" + std::to_string(event.cbutton.button) + "\n");
                    break;
                    default:
                        break;
                }
            }
        }
    }

    void closeController() {
        if (controller != nullptr) {
            SDL_GameControllerClose(controller);
            controller = nullptr;
        }
    }

    void close() {
        closeController();
    }

    ~GameControllerHandler() {
        close();
        SDL_Quit();
    }

private:
    SDL_GameController* controller;
};
