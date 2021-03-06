#include "BreakableBlueWall.h"

BreakableBlueWall::BreakableBlueWall(int x, int y) : BaseObject(BREAKABLE_BLUE_WALL)
{
	_sprite = SpriteManager::getInstance()->getSprite(eID::BREAKABLE_WALL);
	_sprite->setFrameRect(SpriteManager::getInstance()->getSourceRect(eID::BREAKABLE_WALL, "breakable_blue_wall_01"));
	_sprite->setPosition(x, y);

	_animation = new Animation(_sprite, 1.0f);
	_animation->addFrameRect(eID::BREAKABLE_WALL, "breakable_blue_wall_01", NULL);

	_effect = SpriteManager::getInstance()->getSprite(eID::BULLET_EFFECT);
	_effect->setFrameRect(SpriteManager::getInstance()->getSourceRect(eID::BULLET_EFFECT, "explosion_01"));
	_effectAnimation = new Animation(_effect, 0.1f, false);
	_effectAnimation->addFrameRect(BULLET_EFFECT, "explosion_01", "explosion_02", "explosion_03", NULL);

	_hitPoint = 1;
}

void BreakableBlueWall::init()
{
	auto collisionBody = new CollisionBody(this);
	_componentList["CollisionBody"] = collisionBody;

	_effectStopWatch = new StopWatch();
}

void BreakableBlueWall::update(float deltatime)
{
	if (_hitPoint > 0)
	{
		_animation->update(deltatime);

		for (auto it = _componentList.begin(); it != _componentList.end(); it++)
		{
			it->second->update(deltatime);
		}
	}
	else
	{
		_effect->setPosition(this->getPosition());
		_effectAnimation->update(deltatime);

		if (!_effectAnimation->isAnimate())
		{
			if (_effectStopWatch->isStopWatch(5000))
			{
				_hitPoint = 1;
				_effectAnimation->restart();
				_effectStopWatch->restart();
			}
		}
	}
}

void BreakableBlueWall::draw(LPD3DXSPRITE spriteHandle, Viewport* viewport)
{
	if (_hitPoint > 0)
		_animation->draw(spriteHandle, viewport);
	else
	{
		if (_effectAnimation->isAnimate())
			_effectAnimation->draw(spriteHandle, viewport);
	}
}

void BreakableBlueWall::release()
{
	SAFE_DELETE(_animation);
	for (auto it = _componentList.begin(); it != _componentList.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	_componentList.clear();
}

void BreakableBlueWall::wasHit(int hitPoint)
{
	_hitPoint -= hitPoint;
}

bool BreakableBlueWall::isDead()
{
	return (_hitPoint <= 0);
}

float BreakableBlueWall::checkCollision(BaseObject* object, float dt)
{
	return 0;
}
