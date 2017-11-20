#include "MissileRocket.h"

MissileRocket::MissileRocket(int x, int y, eDirection direction) : Weapon(MISSILE_ROCKET)
{
	_sprite = SpriteManager::getInstance()->getSprite(BULLET_EFFECT);
	_sprite->setFrameRect(SpriteManager::getInstance()->getSourceRect(eID::BULLET_EFFECT, "missile_01"));
	_sprite->setPosition(x, y);

	auto movement = new Movement(GVector2(0, 0), GVector2(0, 0), _sprite);
	_componentList["Movement"] = movement;

	switch (direction)
	{
	case RIGHT:
		movement->setVelocity(GVector2(MISSILE_ROCKET_SPEED, 0));
		break;
	case LEFT:
		_sprite->setScaleX(_sprite->getScale().x * (-1));
		movement->setVelocity(GVector2(-MISSILE_ROCKET_SPEED, 0));
		break;
	default:
		break;
	}
}

void MissileRocket::init()
{
	auto collisionBody = new CollisionBody(this);
	_componentList["CollisionBody"] = collisionBody;
}

void MissileRocket::update(float deltatime)
{
	for (auto it = _componentList.begin(); it != _componentList.end(); it++)
	{
		it->second->update(deltatime);
	}
}

void MissileRocket::draw(LPD3DXSPRITE spriteHandle, Viewport* viewport)
{
	_sprite->render(spriteHandle, viewport);
}

void MissileRocket::release()
{
}