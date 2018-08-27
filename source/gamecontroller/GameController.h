/*
 * GameController.h
 *
 *  Created on: Aug 21, 2018
 *      Author: fanick
 */

#ifndef SOURCE_GAMECONTROLLER_GAMECONTROLLER_H_
#define SOURCE_GAMECONTROLLER_GAMECONTROLLER_H_

#include <SDL2/SDL_joystick.h>
#include <string>
#include <array>
#include <iterator>
namespace Duel6 {

class GameController {

public:
    using InstanceID = SDL_JoystickID;
    using Instance = SDL_Joystick *;
    using ControllerGUID = SDL_JoystickGUID;
    using AxisPosition = Sint16;

    GameController(Instance instance);
    virtual ~GameController() = default;

    bool isPressed(int button) const;
    AxisPosition getAxis(int axis) const;
    const ControllerGUID & getGUID() const;
    InstanceID getInstanceID() const;
    const std::string & getName() const;
    void close();
    void reset(Instance instance);
    bool isOpen(){ return open;}
    static InstanceID toInstanceID(Instance instance){
        return SDL_JoystickInstanceID(instance);
    }
    static ControllerGUID toGUID(Instance instance){
        return SDL_JoystickGetGUID(instance);
    }
private:
    bool open = false;
    Instance instance;
    InstanceID instanceID;
    const ControllerGUID guid;
    std::string name;
private:
    GameController(const GameController &) = delete;
    GameController(GameController &&) = delete;
    GameController& operator=(const GameController &) = delete;
};
bool operator ==(const GameController::ControllerGUID & l,const  GameController::ControllerGUID & r);
} /* namespace myengine */

#endif /* SOURCE_GAMECONTROLLER_GAMECONTROLLER_H_ */
