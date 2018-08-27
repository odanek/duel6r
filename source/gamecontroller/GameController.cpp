/*
 * GameController.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: fanick
 */

#include "GameController.h"

namespace Duel6 {

GameController::GameController(Instance instance):
        open(SDL_bool::SDL_TRUE == SDL_JoystickGetAttached(instance)),
        instance(instance),
        instanceID(SDL_JoystickInstanceID(instance)),
        guid(SDL_JoystickGetGUID(instance)),
        name(SDL_JoystickName(instance)){

}

const GameController::ControllerGUID & GameController::getGUID() const {
    return guid;
}

GameController::InstanceID GameController::getInstanceID() const {
    return instanceID;
}

GameController::AxisPosition GameController::getAxis(int axis) const {
    if (!open){
        return 0;
    }
    return SDL_JoystickGetAxis(instance, axis);
}
const std::string & GameController::getName() const {
    return name;
}
bool GameController::isPressed(int button) const {
    return open && (SDL_JoystickGetButton(instance, button) == 1);
}

void GameController::close() {
    open = false;
    SDL_JoystickClose(instance); // This breaks everything, I don't know why
}

void GameController::reset(Instance instance) {
    this->instance = instance;
    open = SDL_bool::SDL_TRUE == SDL_JoystickGetAttached(instance);
    instanceID = SDL_JoystickInstanceID(instance);
}
bool operator ==(const GameController::ControllerGUID & l,const  GameController::ControllerGUID & r){
    for(std::size_t i = 0; i < std::size(l.data); ++i){
        if(l.data[i] != r.data[i]){
            return false;
        }
    }
    return true;
}
} /* namespace myengine */
