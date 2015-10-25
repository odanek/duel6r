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

#include "ShotBase.h"
#include "../World.h"
#include "../collision/Collision.h"

namespace Duel6
{
	ShotBase::ShotBase(Player& player, SpriteList::Iterator sprite, Orientation shotOrientation)
		: player(player), weapon(player.getWeapon()), orientation(shotOrientation), sprite(sprite)
	{
		const Vector dim = getDimensions();
		const Rectangle playerRect = player.getCollisionRect();
		if (orientation == Orientation::Left)
		{
			position = Vector(playerRect.left.x - dim.x, player.getGunVerticalPosition() - dim.y / 2.0f);
			velocity = Vector(-1.0f, 0.0f);
		}
		else
		{
			position = Vector(playerRect.right.x, player.getGunVerticalPosition() - dim.y / 2.0f);
			velocity = Vector(1.0f, 0.0f);
		}
		this->sprite->setPosition(getSpritePosition(), 0.6f).setOrientation(this->orientation);
		powerful = getPlayer().hasPowerfulShots();
	}

	void ShotBase::move(Float32 elapsedTime)
	{
		position += velocity * getWeapon().bulletSpeed * elapsedTime;
		sprite->setPosition(getSpritePosition());
	}

	Float32 ShotBase::getExplosionPower() const
	{
		Float32 coef = (powerful ? 2.0f : 1.0f);
		return getWeapon().power * coef;
	}

	Float32 ShotBase::getExplosionRange() const
	{
		Float32 coef = (powerful ? 2.0f : 1.0f);
		return getWeapon().boom * coef;
	}

	bool ShotBase::update(Float32 elapsedTime, World& world)
	{
		move(elapsedTime);

		Hit hit = checkPlayerCollision(world.getPlayers());
		if (!hit.hit)
		{
			hit = checkWorldCollision(world.getLevel());
		}

		if (hit.hit)
		{
			explode(hit, world);

			Sprite boom(weapon.boomAnimation, weapon.textures.boom);
			boom.setPosition(getCentre() - Vector(0.5f, 0.5f), 0.6f)
				.setSpeed(2.0f)
				.setLooping(AnimationLooping::OnceAndRemove)
				.setOrientation(orientation)
				.setAlpha(0.6f)
				.setGrow(weapon.expGrow * (powerful ? 1.2f : 1.0f));

			SpriteList::Iterator boomSprite = world.getSpriteList().addSprite(boom);
			weapon.boomSample.play();
			if (weapon.boom > 0)
			{
				boomSprite->setNoDepth(true);
			}

			world.getSpriteList().removeSprite(sprite);
			return false;
		}
		return true;
	}

	void ShotBase::explode(Hit hit, World& world)
	{
		std::vector<Player*> killedPlayers;
		std::vector<Player*> hittedPlayers;

		Player& author = getPlayer();

		Float32 range = getExplosionRange();
		Float32 power = getExplosionPower();

		const Vector shotCentre = getCentre();
		bool shit = getWeapon().shit;

		if (!shit)
		{
			world.getFireList().check(shotCentre, range);
		}

		for (Player& player : world.getPlayers())
		{
			bool directHit = (hit.player != nullptr && player.is(*hit.player));
			Vector playerCentre = player.getCentre();
			Float32 dist = directHit ? 0 : (playerCentre - shotCentre).length();

			if (directHit || dist < range)
			{
				hittedPlayers.push_back(&player);
				if (shit)
				{
					player.useTemporarySkin(*d6BrownSkin);
				}
				else
				{
					if (player.hitByShot(directHit ? power : ((range - dist) * power) / range, *this, directHit, getCentre()))
					{
						killedPlayers.push_back(&player);
					}
				}
			}
		}

		author.processShot(*this, hittedPlayers, killedPlayers);
	}

	ShotBase::Hit ShotBase::checkPlayerCollision(std::vector<Player>& players)
	{
		const Rectangle shotBox = getCollisionRect();

		for (Player& player : players)
		{
			if (!player.isInGame() || player.getBonus() == D6_BONUS_INVIS || player.is(getPlayer()))
			{
				continue;
			}

			if (Collision::rectangles(player.getCollisionRect(), shotBox))
			{
				return {true, &player};
			}
		}

		return { false, nullptr };
	}

	ShotBase::Hit ShotBase::checkWorldCollision(const Level& level)
	{
		const Rectangle box = getCollisionRect();
		Float32 up = box.right.y;
		Float32 down = box.left.y;
		Float32 left = box.left.x;
		Float32 right = box.right.x;

		bool hitsWall = level.isWall(left, up, true) ||
						level.isWall(left, down, true) ||
						level.isWall(right, up, true) ||
						level.isWall(right, down, true);

		return { hitsWall, nullptr };
	}
}
