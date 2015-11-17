/*
* Copyright (c) 2006, Ondrej Danek (www.ondrej-danek.net)
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Ondrej Danek nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
* GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <SDL2/SDL_opengl.h>
#include "Player.h"
#include "ElevatorList.h"
#include "json/JsonParser.h"

namespace Duel6
{
	ElevatorList::ElevatorList(const TextureList& textures)
		: textures(textures)
	{}

	void ElevatorList::add(Elevator& elevator)
	{
		elevators.push_back(elevator);
		elevators.back().start();
	}

	void ElevatorList::load(const std::string& path, bool mirror)
	{
		Json::Parser parser;
		Json::Value root = parser.parse(path);

		Int32 width = root.get("width").asInt();
		Int32 height = root.get("height").asInt();

		Size elevators = root.get("elevators").getLength();
		for (Size i = 0; i < elevators; i++) {
			Elevator elevator;
			Json::Value points = root.get("elevators").get(i).get("controlPoints");
			for (Size j = 0; j < points.getLength(); j++) {
				Int32 x = points.get(j).get("x").asInt();
				Int32 y = points.get(j).get("y").asInt();
				elevator.addControlPoint(Elevator::ControlPoint(mirror ? width - 1 - x : x, height - y));
			}
			add(elevator);
		}
	}

	void ElevatorList::update(Float32 elapsedTime)
	{
		for (Elevator& elevator : elevators)
		{
			elevator.update(elapsedTime);
		}
	}

	void ElevatorList::render() const
	{
		glBindTexture(GL_TEXTURE_2D, textures.at(0).getId());
		glBegin(GL_QUADS);

		for (const Elevator& elevator : elevators)
		{
			elevator.render();
		}

		glEnd();
	}

	const Elevator* ElevatorList::checkPlayer(Player& player)
	{
		Rectangle playerRect = player.getCollisionRect();
		Float32 cX = playerRect.getCentre().x;

		for (const Elevator& elevator : elevators)
		{
			const Vector& pos = elevator.getPosition();
			if (cX >= pos.x && cX <= pos.x + 1.0f && playerRect.left.y >= pos.y - 0.05f && playerRect.left.y <= pos.y + 0.05f)  // TODO: Coord
			{
				return &elevator;
			}
		}

		return nullptr;
	}
}
