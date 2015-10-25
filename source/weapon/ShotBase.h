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

#ifndef DUEL6_SHOTBASE_H
#define DUEL6_SHOTBASE_H

#include "../Shot.h"
#include "../Weapon.h"
#include "../Vector.h"
#include "../SpriteList.h"
#include "../Player.h"

namespace Duel6
{
	class ShotBase : public Shot
	{
	protected:
		struct Hit
		{
			bool hit;
			Player* player;
		};

	private:
		Player& player;
		const Weapon& weapon;
		Orientation orientation;
		Vector position;
		Vector velocity;
		SpriteList::Iterator sprite;
		bool powerful;

	public:
		ShotBase(Player& player, SpriteList::Iterator sprite, Orientation shotOrientation);

		Player& getPlayer() override
		{
			return player;
		}

		const Player& getPlayer() const override
		{
			return player;
		}

		const Weapon& getWeapon() const override
		{
			return weapon;
		}

		bool update(Float32 elapsedTime, World& world) override;

	protected:
		const Vector& getPosition() const
		{
			return position;
		}

		Vector getDimensions() const
		{
			return Vector(0.65f, 0.35f);
		}

		Vector getCentre() const
		{
			return getCollisionRect().getCentre();
		}

		Rectangle getCollisionRect() const
		{
			return Rectangle::fromCornerAndSize(getPosition(), getDimensions());
		}

		Vector getSpritePosition() const
		{
			return orientation == Orientation::Left ? Vector(position.x, position.y - 0.65f) : Vector(position.x - 0.35f, position.y - 0.65f);
		}

		void move(Float32 elapsedTime);

		Float32 getExplosionRange() const;
		Float32 getExplosionPower() const;

		Hit checkPlayerCollision(std::vector<Player>& players);
		Hit checkWorldCollision(const Level& level);
		void explode(Hit hit, World& world);
	};
}

#endif
