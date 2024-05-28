#include "GameControllerHandler.h"

GameControllerHandler::GameControllerHandler(const char *ip, int port) : TCPClient(ip, port), controller(nullptr) {}

bool GameControllerHandler::init() {
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

void GameControllerHandler::start() {
    TCPClient::start();

    std::thread t(&GameControllerHandler::handleEvents, this);
    t.detach();

    this->sendMessage("gc;strat;ready;1\n");
}

void GameControllerHandler::handleMessage(const std::string &message) {
    std::vector<std::string> tokens = Modelec::split(message, ";");

    if (tokens[1] == "all" || tokens[1] == "lidar") {
        if (tokens[2] == "set range") {
            this->lidarDectectionDistance = stod(tokens[3]);
        }
    }

    if (tokens[1] == "all" || tokens[1] == "gc") {
        if (tokens[2] == "stop proximity") {
            if (!this->rumble) return;

            std::vector<std::string> args = Modelec::split(tokens[3], ",");
            double distance = stod(args[0]);
            Uint16 strength;

            if (distance <= 100) {
                strength = 0xFFFF;
            } else if (distance <= 300) {
                double factor = 1 - Modelec::mapValue(distance, 100.0, this->lidarDectectionDistance, 0.0, 1.0);
                strength = static_cast<Uint16>(factor * 0xFFFF);
            } else {
                strength = 0;
            }

            if (SDL_GameControllerRumble(controller, strength, strength, 1000) != 0) {
                std::cerr << "Erreur lors de l'activation de la vibration : " << SDL_GetError() << std::endl;
            }
        }

        if (tokens[2] == "ready") {
            this->sendMessage("gc;all;game mode;gc\n");
        }

        if (tokens[2] == "start proximity alert") {
            this->rumble = true;
        }
        else if (tokens[2] == "stop proximity alert") {
            this->rumble = false;
            if (SDL_GameControllerRumble(controller, 0x0, 0x0, 1000) != 0) {
                std::cerr << "Erreur lors de l'activation de la vibration : " << SDL_GetError() << std::endl;
            }
        }
    }
}

void GameControllerHandler::handleEvents() {
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    this->sendMessage("gc;strat;disconnect;1\n");
                break;
                case SDL_CONTROLLERAXISMOTION:
                    if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
                        std::cout << "Trigger left moved to " << event.caxis.value << std::endl;
                        this->sendMessage("gc;strat;trigger;0," + std::to_string(event.caxis.value) + "\n");
                    } else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
                        std::cout << "Trigger right moved to " << event.caxis.value << std::endl;
                        this->sendMessage("gc;strat;trigger;1," + std::to_string(event.caxis.value) + "\n");
                    }
                    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                        std::cout << "Left X axis moved to " << event.caxis.value << std::endl;
                        this->sendMessage("gc;strat;axis;0," + std::to_string(event.caxis.value) + "\n");
                    }
                    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                        std::cout << "Left Y axis moved to " << event.caxis.value << std::endl;
                        this->sendMessage("gc;strat;axis;1," + std::to_string(event.caxis.value) + "\n");
                    }
                    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                        std::cout << "Right X axis moved to " << event.caxis.value << std::endl;
                        this->sendMessage("gc;strat;axis;2," + std::to_string(event.caxis.value) + "\n");
                    }
                    else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
                        std::cout << "Right Y axis moved to " << event.caxis.value << std::endl;
                        // this->sendMessage("gc;strat;axis;3," + std::to_string(event.caxis.value) + "\n");
                    }
                break;
                case SDL_CONTROLLERBUTTONDOWN:
                    std::cout << "Button down " << static_cast<int>(event.cbutton.button) << std::endl;
                    this->sendMessage("gc;strat;button down;" + std::to_string(event.cbutton.button) + "\n");
                break;
                case SDL_CONTROLLERBUTTONUP:
                    std::cout << "Button up " << static_cast<int>(event.cbutton.button) << std::endl;
                    this->sendMessage("gc;strat;button up;" + std::to_string(event.cbutton.button) + "\n");
                break;
                default:
                    break;
            }
            usleep(5);
        }
    }
}

void GameControllerHandler::closeController() {
    if (controller != nullptr) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }
}

void GameControllerHandler::close() {
    closeController();
}

GameControllerHandler::~GameControllerHandler() {
    close();
    SDL_Quit();
}
