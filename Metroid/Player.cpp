﻿#include "Player.h"
#include "SceneManager.h"

Player::Player() : BaseObject(eID::PLAYER)
{
}

Player::~Player()
{
}

void Player::init()
{
	__hook(&InputController::__eventkeyPressed, _input, &Player::onKeyPressed);
	__hook(&InputController::__eventkeyReleased, _input, &Player::onKeyReleased);

	_sprite = SpriteManager::getInstance()->getSprite(eID::PLAYER);
	_sprite->setFrameRect(SpriteManager::getInstance()->getSourceRect(eID::PLAYER, "normal"));
	_sprite->setZIndex(1.0f);

	auto movement = new Movement(GVector2(0, 0), GVector2(0, 0), _sprite);
	_componentList["Movement"] = movement;
	_componentList["Gravity"] = new Gravity(GVector2(0, -GRAVITY), movement);

	auto collisionBody = new CollisionBody(this);
	_componentList["CollisionBody"] = collisionBody;

	_animations[eStatus::NORMAL] = new Animation(_sprite, 0.07f);
	_animations[eStatus::NORMAL]->addFrameRect(eID::PLAYER, "normal", NULL);

	_animations[eStatus::NORMAL | eStatus::ATTACKING] = new Animation(_sprite, 0.1f);
	_animations[eStatus::NORMAL | eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "normal", NULL);

	_animations[eStatus::RUNNING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::RUNNING]->addFrameRect(eID::PLAYER, "walk_right_01", "walk_right_02", "walk_right_03", NULL);

	_animations[eStatus::JUMPING | eStatus::NORMAL] = new Animation(_sprite, 0.07f);
	_animations[eStatus::JUMPING | eStatus::NORMAL]->addFrameRect(eID::PLAYER, "jump", NULL);

	_animations[eStatus::FALLING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::FALLING]->addFrameRect(eID::PLAYER, "normal", NULL);

	_animations[eStatus::LOOKING_UP] = new Animation(_sprite, 0.07f);
	_animations[eStatus::LOOKING_UP]->addFrameRect(eID::PLAYER, "look_up", NULL);

	_animations[eStatus::ROLLING_DOWN] = new Animation(_sprite, 0.07f);
	_animations[eStatus::ROLLING_DOWN]->addFrameRect(eID::PLAYER, "roll_down_01", "roll_down_02", "roll_down_03", "roll_down_04", NULL);

	_animations[eStatus::LOOKING_UP | eStatus::ATTACKING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::LOOKING_UP | eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "look_up", NULL);

	_animations[eStatus::LOOKING_UP | eStatus::RUNNING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::LOOKING_UP | eStatus::RUNNING]->addFrameRect(eID::PLAYER, "run_look_up_01", "run_look_up_02", "run_look_up_03", NULL);

	_animations[eStatus::LOOKING_UP | eStatus::RUNNING | eStatus::ATTACKING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::LOOKING_UP | eStatus::RUNNING | eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "run_look_up_01", "run_look_up_02", "run_look_up_03", NULL);

	_animations[eStatus::LOOKING_UP | eStatus::RUNNING | eStatus::JUMPING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::LOOKING_UP | eStatus::RUNNING | eStatus::JUMPING]->addFrameRect(eID::PLAYER, "jump", NULL);

	_animations[eStatus::LOOKING_UP | eStatus::JUMPING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::LOOKING_UP | eStatus::JUMPING]->addFrameRect(eID::PLAYER, "jump_look_up", NULL);

	_animations[eStatus::LOOKING_UP | eStatus::JUMPING | eStatus::ATTACKING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::LOOKING_UP | eStatus::JUMPING | eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "jump_look_up", NULL);

	_animations[eStatus::RUNNING | eStatus::LOOKING_UP | eStatus::JUMPING | eStatus::ATTACKING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::RUNNING | eStatus::LOOKING_UP | eStatus::JUMPING | eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "jump_look_up", NULL);

	_animations[eStatus::ROLLING_DOWN | eStatus::RUNNING] = new Animation(_sprite, 0.01f);
	_animations[eStatus::ROLLING_DOWN | eStatus::RUNNING]->addFrameRect(eID::PLAYER, "roll_down_01", "roll_down_02", "roll_down_03", "roll_down_04", NULL);

	_animations[eStatus::RUNNING | eStatus::JUMPING] = new Animation(_sprite, 0.04f);
	_animations[eStatus::RUNNING | eStatus::JUMPING]->addFrameRect(eID::PLAYER, "run_jump_01", "run_jump_02", "run_jump_03", "run_jump_04", NULL);

	_animations[eStatus::RUNNING | eStatus::JUMPING | eStatus::ATTACKING] = new Animation(_sprite, 0.05f);
	_animations[eStatus::RUNNING | eStatus::JUMPING | eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "jump_attack", NULL);

	_animations[eStatus::ATTACKING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "normal", NULL);

	_animations[eStatus::ATTACKING | eStatus::RUNNING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::ATTACKING | eStatus::RUNNING]->addFrameRect(eID::PLAYER, "run_attack_01", "run_attack_02", "run_attack_03", NULL);

	_animations[eStatus::JUMPING | eStatus::ATTACKING] = new Animation(_sprite, 0.07f);
	_animations[eStatus::JUMPING | eStatus::ATTACKING]->addFrameRect(eID::PLAYER, "jump_attack", NULL);

	// Animation DIE không lặp lại, để khi chạy hết animation này thì revive
	_animations[eStatus::DIE] = new Animation(_sprite, 0.5f, false);
	_animations[eStatus::DIE]->addFrameRect(eID::PLAYER, "roll_down_01", "roll_down_02", "roll_down_03", "roll_down_04", NULL);

	// Khởi tạo StopWatch
	_attackStopWatch = new StopWatch();

	// Set origin của nhân vật ở giữa, phía dưới
	this->setOrigin(GVector2(0.5f, 0.0f));
	this->setStatus(eStatus::NORMAL);

	_currentAnimateIndex = NORMAL;
	_isRevive = false;

	// Info dùng tọa độ top-left
	_info = new Info();
	_info->init();
	_info->setLife(2);
	_info->setEnergy(30);

	this->resetValues();
}

void Player::update(float deltatime)
{
	if (_info->getEnergy() <= 0)
	{
		this->die();
		_protectTime = 0;
	}

	if (_protectTime > 0)
	{
		_protectTime -= deltatime;
	}

	// Từ status để gọi hành động
	this->updateStatus(deltatime);

	// Từ status để gọi attack
	this->updateAttackStatus(deltatime);

	// Từ status để chuyển animation
	this->updateCurrentAnimateIndex();

	_animations[_currentAnimateIndex]->update(deltatime);

	// Update các component
	for (auto it = _componentList.begin(); it != _componentList.end(); it++)
	{
		it->second->update(deltatime);
	}

	// Update các viên đạn, bomb
	for (auto weapon : _listWeapon)
	{
		weapon->update(deltatime);
	}

	this->_info->update(deltatime);
}

void Player::updateInput(float dt)
{
	// Dùng event
}

void Player::draw(LPD3DXSPRITE spriteHandle, Viewport* viewport)
{
	if (_protectTime > 0)
		_animations[_currentAnimateIndex]->enableFlashes(true);
	else
		_animations[_currentAnimateIndex]->enableFlashes(false);

	_animations[_currentAnimateIndex]->draw(spriteHandle, viewport);

	for (auto weapon : _listWeapon)
	{
		weapon->draw(spriteHandle, viewport);
	}

	_info->draw(spriteHandle, viewport);
}

void Player::release()
{
	for (auto it = _animations.begin(); it != _animations.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	_animations.clear();

	for (auto it = _componentList.begin(); it != _componentList.end(); it++)
	{
		SAFE_DELETE(it->second);
	}
	_componentList.clear();

	SAFE_DELETE(_sprite);
	SAFE_DELETE(_attackStopWatch);
	SAFE_DELETE(_info);

	if (_input != nullptr)
		__unhook(_input);
}

void Player::updateStatus(float dt)
{
	if (this->isInStatus(eStatus::DIE))
	{
		// Lưu lại vị trí chết để set vị trí hồi sinh
		this->_revivePosition = GVector2(this->getPosition());

		if (_info->getLife() < 0)
			return;

		// Khi chạy hết DIE animation thì revive
		if (!_animations[eStatus::DIE]->isAnimate())
			this->revive();
	}

	if ((this->getStatus() & eStatus::MOVING_LEFT) == eStatus::MOVING_LEFT)
	{
		this->moveLeft();
	}
	else if ((this->getStatus() & eStatus::MOVING_RIGHT) == eStatus::MOVING_RIGHT)
	{
		this->moveRight();
	}
	else if ((this->getStatus() & eStatus::FALLING) == eStatus::FALLING)
	{
		this->falling();
	}
	else if ((this->getStatus() & eStatus::JUMPING) != eStatus::JUMPING)
	{
		this->standing();
	}

	if ((this->getStatus() & eStatus::ATTACKING) == eStatus::ATTACKING)
	{
		// Nếu đang ROLLING_DOWN thì đổi thành BOMB, còn lại giữ nguyên BULLET
		if ((this->getStatus() & eStatus::ROLLING_DOWN) == eStatus::ROLLING_DOWN)
		{
			// Nếu có Bomb thì mới được đặt bomb
			if (_info->hasBomb())
				_info->setWeapon(eID::BOMB);
		}
	}
}

void Player::updateAttackStatus(float dt)
{
	if (this->isInStatus(eStatus::ATTACKING))
	{
		if (_attackStopWatch->isStopWatch(ATTACK_TIME))
		{
			Weapon* weapon = nullptr;
			switch (_info->getWeapon())
			{
			case NORMAL_BULLET:
			{
				if (this->isInStatus(eStatus::LOOKING_UP))
				{
					if (this->getScale().x > 0)
						weapon = new NormalBullet(this->getPositionX() + 6, this->getPositionY() + 70, eDirection::TOP, _info->getBulletRange());
					else
						weapon = new NormalBullet(this->getPositionX() - 4, this->getPositionY() + 70, eDirection::TOP, _info->getBulletRange());
				}
				else
				{
					if (this->getScale().x > 0)
						weapon = new NormalBullet(this->getPositionX() + 16, this->getPositionY() + 40, eDirection::RIGHT, _info->getBulletRange());
					else
						weapon = new NormalBullet(this->getPositionX() - 16, this->getPositionY() + 40, eDirection::LEFT, _info->getBulletRange());
				}
				break;
			}
			case ICE_BULLET:
			{
				if (this->isInStatus(eStatus::LOOKING_UP))
				{
					if (this->getScale().x > 0)
						weapon = new IceBullet(this->getPositionX() + 6, this->getPositionY() + 70, eDirection::TOP, _info->getBulletRange());
					else
						weapon = new IceBullet(this->getPositionX() - 4, this->getPositionY() + 70, eDirection::TOP, _info->getBulletRange());
				}
				else
				{
					if (this->getScale().x > 0)
						weapon = new IceBullet(this->getPositionX() + 16, this->getPositionY() + 40, eDirection::RIGHT, _info->getBulletRange());
					else
						weapon = new IceBullet(this->getPositionX() - 16, this->getPositionY() + 40, eDirection::LEFT, _info->getBulletRange());
				}
				break;
			}
			case MISSILE_ROCKET:
			{
				if (this->isInStatus(eStatus::LOOKING_UP))
				{
					if (this->getScale().x > 0)
						weapon = new MissileRocket(this->getPositionX() + 6, this->getPositionY() + 70, eDirection::TOP);
					else
						weapon = new MissileRocket(this->getPositionX() - 4, this->getPositionY() + 70, eDirection::TOP);
				}
				else
				{
					if (this->getScale().x > 0)
						weapon = new MissileRocket(this->getPositionX() + 16, this->getPositionY() + 40, eDirection::RIGHT);
					else
						weapon = new MissileRocket(this->getPositionX() - 16, this->getPositionY() + 40, eDirection::LEFT);
				}

				// Nếu hết Rocket thì set về Bullet
				_info->setMissileRocket(_info->getMissileRocket() - 1);
				if (_info->getMissileRocket() <= 0)
					_info->setWeapon(_info->getBullet());
				break;
			}
			case BOMB:
				weapon = new Bomb(this->getPositionX(), this->getPositionY() + 10);
				break;
			default:
				break;
			}

			if (weapon != nullptr)
			{
				weapon->init();
				_listWeapon.push_back(weapon);
			}

			_attackStopWatch->restart();
		}
	}

	// Xóa các viên đạn, bomb đã bị DESTROY
	if (!_listWeapon.empty())
	{
		auto i = 0;
		while (i < _listWeapon.size())
		{
			auto object = _listWeapon[i];
			
			if (_listWeapon[i]->isInStatus(DESTROY))
			{
				_listWeapon.erase(_listWeapon.begin() + i);
				object->release();
				delete object;
			}
			else
				i++;
		}
	}
}

void Player::updateCurrentAnimateIndex()
{
	_currentAnimateIndex = this->getStatus();

	if (!((_currentAnimateIndex & eStatus::ROLLING_DOWN) == eStatus::ROLLING_DOWN) && (_currentAnimateIndex & eStatus::FALLING) == eStatus::FALLING)
	{
		_currentAnimateIndex = eStatus::FALLING;
	}

	if ((_currentAnimateIndex & eStatus::ROLLING_DOWN) == eStatus::ROLLING_DOWN)
	{
		_currentAnimateIndex = eStatus::ROLLING_DOWN;
	}

	// Nếu đang trong status MOVING_LEFT hoặc MOVING_RIGHT thì xóa status đó, thay bằng RUNNING
	if ((_currentAnimateIndex & eStatus::MOVING_LEFT) == eStatus::MOVING_LEFT || ((_currentAnimateIndex & eStatus::MOVING_RIGHT) == eStatus::MOVING_RIGHT))
	{
		_currentAnimateIndex = (eStatus)(_currentAnimateIndex & ~(eStatus::MOVING_LEFT | MOVING_RIGHT));
		_currentAnimateIndex = (eStatus)(_currentAnimateIndex | eStatus::RUNNING);
	}

	if (this->isInStatus(eStatus::DIE))
	{
		_currentAnimateIndex = eStatus::DIE;
	}
}

bool Player::checkWeaponCollision(BaseObject* object, eDirection& direction, eID& weaponID, float dt)
{
	if (!_listWeapon.empty())
	{
		auto i = 0;
		while (i < _listWeapon.size())
		{
			// Nếu có va chạm với object (là enemy, hoặc wall) đang xét thì xóa đạn, bomb đang xét ra khỏi list
			// đồng thời return TRUE
			auto weaponCollision = _listWeapon[i]->getCollisionBody();
			if (weaponCollision->checkCollision(object, direction, dt, false))
			{
				// Lưu lại weaponID để xét loại weapon sau này
				weaponID = _listWeapon[i]->getId();

				auto object = _listWeapon[i];
				_listWeapon.erase(_listWeapon.begin() + i);
				object->release();
				delete object;
				
				return true;
			}
			i++;
		}
	}
	return false;
}

void Player::onKeyPressed(KeyEventArg* keyEvent)
{
	if (this->isInStatus(eStatus::DIE))
		return;

	switch (keyEvent->keyCode)
	{
	case DIK_LEFT:
	{
		this->removeStatus(eStatus::MOVING_RIGHT);
		this->addStatus(eStatus::MOVING_LEFT);
		break;
	}
	case DIK_RIGHT:
	{
		this->removeStatus(eStatus::MOVING_LEFT);
		this->addStatus(eStatus::MOVING_RIGHT);
		break;
	}
	case DIK_UP:
	{	
		// Nếu đang không trong status ROLLING_DOWN, JUMPING và FALLING thì ngắm hướng lên (LOOKING_UP)
		if (!this->isInStatus(eStatus::ROLLING_DOWN) && !this->isInStatus(eStatus::JUMPING) && !this->isInStatus(eStatus::FALLING))
		{
			this->addStatus(eStatus::LOOKING_UP);
		}
		else // Nếu đang trong status ROLLING_DOWN thì đứng dậy (xóa status ROLLING_DOWN)
		{
			this->removeStatus(eStatus::ROLLING_DOWN);
			_info->setWeapon(_info->getBullet());
		}
		break;
	}
	case DIK_DOWN:
	{
		if (!this->isInStatus(eStatus::JUMPING) && !this->isInStatus(eStatus::FALLING))
		{
			// Nếu có MaruMari mới được ROLLING_DOWN
			if (_info->hasMaruMari())
				this->addStatus(eStatus::ROLLING_DOWN);
		}
		break;
	}
	case DIK_X:
	{
		if (!this->isInStatus(eStatus::ROLLING_DOWN) && ((this->isInStatus(eStatus::NORMAL) || this->isInStatus(eStatus::MOVING_LEFT) || this->isInStatus(eStatus::MOVING_RIGHT))))
			this->jump();

		// Nếu đang trong status ROLLING_DOWN thì đứng dậy (xóa status ROLLING_DOWN)
		if (this->isInStatus(eStatus::ROLLING_DOWN))
		{
			this->removeStatus(eStatus::ROLLING_DOWN);
			_info->setWeapon(_info->getBullet());
		}
		break;
	}
	case DIK_Z:
	{
		// Nếu đang ROLLING_DOWN mà không có Bomb thì không cho attack
		if (!(this->isInStatus(eStatus::ROLLING_DOWN) && !_info->hasBomb()))
			this->addStatus(eStatus::ATTACKING);
		break;
	}
	case DIK_SPACE:
		// Chuyển giữa Rocket và Bullet, nếu đang ROLLING_DOWN thì không cho chuyển
		if ((_info->getMissileRocket() > 0) && ((this->getStatus() & eStatus::ROLLING_DOWN) != eStatus::ROLLING_DOWN))
		{
			if (_info->getWeapon() != eID::MISSILE_ROCKET)
				_info->setWeapon(eID::MISSILE_ROCKET);
			else
				_info->setWeapon(_info->getBullet());
		}
		break;
	default:
		break;
	}
}

void Player::onKeyReleased(KeyEventArg* keyEvent)
{
	if (this->isInStatus(eStatus::DIE))
		return;

	switch (keyEvent->keyCode)
	{
	case DIK_LEFT:
	{
		this->removeStatus(eStatus::MOVING_LEFT);
		this->removeStatus(eStatus::RUNNING);
		break;
	}
	case DIK_RIGHT:
	{
		this->removeStatus(eStatus::MOVING_RIGHT);
		this->removeStatus(eStatus::RUNNING);
		break;
	}
	case DIK_UP:
	{
		this->removeStatus(eStatus::LOOKING_UP);
		break;
	}
	case DIK_DOWN:
	{
		break;
	}
	case DIK_X:
	{
		break;
	}
	case DIK_Z:
	{
		this->removeStatus(eStatus::ATTACKING);
		break;
	}
	default:
		break;
	}
}

void Player::resetValues()
{
	_listWeapon.clear();
	preWall = nullptr;

	this->setScale(SCALE_FACTOR);
	_movingSpeed = MOVE_SPEED;
	_protectTime = PROTECT_TIME;

	// Set các giá trị khi hồi sinh (life, energy,...)
	if (_isRevive)
	{
		this->setStatus(eStatus::NORMAL);
		_info->setLife(_info->getLife() - 1);
		_info->setEnergy(30);
		_info->setWeapon(_info->getBullet());

		auto gravity = (Gravity*)this->_componentList["Gravity"];
		gravity->setStatus(eGravityStatus::FALLING_DOWN);

		// Set position player tại vị trí hồi sinh
		this->setPosition(_revivePosition);
		_isRevive = false;
	}

	auto movement = (Movement*)this->_componentList["Movement"];
	movement->setVelocity(GVector2(0, 0));

	for (auto animate : _animations)
	{
		animate.second->setColorFlash(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
}

void Player::standing()
{
	auto movement = (Movement*)this->_componentList["Movement"];
	movement->setVelocity(GVector2(0, 0));

	this->removeStatus(eStatus::JUMPING);
	this->removeStatus(eStatus::FALLING);
}

void Player::moveLeft()
{
	if (this->getScale().x > 0)
		this->setScaleX(this->getScale().x * (-1));

	auto movement = (Movement*)this->_componentList["Movement"];
	movement->setVelocity(GVector2(-_movingSpeed, movement->getVelocity().y));
}

void Player::moveRight()
{
	if (this->getScale().x < 0)
		this->setScaleX(this->getScale().x * (-1));

	auto movement = (Movement*)this->_componentList["Movement"];
	movement->setVelocity(GVector2(_movingSpeed, movement->getVelocity().y));
}

void Player::jump()
{
	if (this->isInStatus(eStatus::JUMPING) || this->isInStatus(eStatus::FALLING))
		return;

	this->addStatus(eStatus::JUMPING);

	auto movement = (Movement*)this->_componentList["Movement"];
	movement->setVelocity(GVector2(movement->getVelocity().x, JUMP_VELOCITY));

	this->falling();
}

void Player::falling()
{
	auto gravity = (Gravity*)this->_componentList["Gravity"];
	gravity->setStatus(eGravityStatus::FALLING_DOWN);
}

void Player::beHit(eDirection direction)
{
	_protectTime = PROTECT_TIME;

	if (direction == NONE)
	{
		_info->setEnergy(_info->getEnergy() - 2);
		return;
	}

	auto gravity = (Gravity*)this->_componentList["Gravity"];
	gravity->setStatus(eGravityStatus::FALLING_DOWN);

	auto movement = (Movement*)this->_componentList["Movement"];

	switch (direction)
	{
	case RIGHT:
	{
		// Nếu va chạm RIGHT thì bị văng sang bên phải (player hướng sang trái)
		if (this->getScale().x > 0)
			this->setScaleX(this->getScale().x * (-1));
		movement->setVelocity(GVector2(MOVE_SPEED / 2, JUMP_VELOCITY / 2));
		break;
	}
	case LEFT:
	{
		// Nếu va chạm LEFT thì bị văng sang bên trái (player hướng sang phải)
		if (this->getScale().x < 0)
			this->setScaleX(this->getScale().x * (-1));
		movement->setVelocity(GVector2(-MOVE_SPEED / 2, JUMP_VELOCITY / 2));
		break;
	}
	default:
	{
		if (this->getScale().x > 0)
			movement->setVelocity(GVector2(-MOVE_SPEED / 2, JUMP_VELOCITY / 2));
		else
			movement->setVelocity(GVector2(MOVE_SPEED / 2, JUMP_VELOCITY / 2));
		break;
	}
	}
}

void Player::die()
{
	if (!this->isInStatus(eStatus::DIE))
		this->setStatus(eStatus::DIE);
}

void Player::revive()
{
	this->_isRevive = true;
	this->resetValues();

	// Restart lại DIE animation do DIE animation không lặp lại
	_animations[eStatus::DIE]->restart();

	this->jump();
}

float Player::checkCollision(BaseObject* object, float dt)
{
	if (object->getStatus() == eStatus::DESTROY || this->isInStatus(eStatus::DIE))
		return 0.0f;
	if (this == object)
		return 0.0f;

	auto collisionBody = (CollisionBody*)_componentList["CollisionBody"];
	eID objectId = object->getId();
	eDirection direction;

	if (objectId == eID::WALL)
	{
		// Kiểm tra va chạm
		if (collisionBody->checkCollision(object, direction, dt, false))
		{
			// Nếu đang va chạm thì dời ra xa
			float moveX, moveY;
			if (collisionBody->isColliding(object, moveX, moveY, dt))
			{
				collisionBody->updateTargetPosition(object, direction, false, GVector2(moveX, moveY));
			}

			// Nếu va chạm TOP mà trừ trường hợp nhảy lên vận tốc y lớn hơn 0
			// Trường hợp vận tốc y lớn hơn 0 là trường hợp nhảy từ dưới lên
			if (direction == eDirection::TOP && !(this->getVelocity().y > 0 && this->isInStatus(eStatus::JUMPING)))
			{
				auto gravity = (Gravity*)this->_componentList["Gravity"];
				gravity->setStatus(eGravityStatus::SHALLOWED);

				this->standing();
				preWall = object;
			}
		}
		else if (preWall == object) // Xét sau va chạm
		{	
			// Nếu đã đi ra khỏi hết Wall đụng trước đó thì cho rớt xuống
			auto gravity = (Gravity*)this->_componentList["Gravity"];
			gravity->setStatus(eGravityStatus::FALLING_DOWN);
			
			// Thêm status FALLING để cho player rớt xuống
			if (!this->isInStatus(eStatus::JUMPING) && !this->isInStatus(eStatus::FALLING))
				this->addStatus(eStatus::FALLING);
		}

		eID weaponId;
		if (this->checkWeaponCollision(object, direction, weaponId, dt))
		{
			// Xét va chạm để khi đạn trúng tường thì bị DESTROY
		}
	}
	else if (objectId == RIPPER)
	{
		if (!((Ripper*)object)->isDead() && _protectTime <= 0)
		{
			if (collisionBody->checkCollision(object, direction, dt, false))
			{
				// Nếu đang va chạm thì dời ra xa
				float moveX, moveY;
				if (collisionBody->isColliding(object, moveX, moveY, dt))
				{
					collisionBody->updateTargetPosition(object, direction, false, GVector2(moveX, moveY));
				}

				beHit(direction);
				_info->setEnergy(_info->getEnergy() - 8);
			}

			eID weaponId;
			if (this->checkWeaponCollision(object, direction, weaponId, dt))
			{
				if (weaponId == eID::MISSILE_ROCKET)
					((Ripper*)object)->wasHit(5);
			}
		}
	}
	else if (objectId == WAVER)
	{
		// Lại gần thì active
		if (!((Waver*)object)->isActive())
		{
			auto objPosition = object->getPosition();
			auto position = this->getPosition();
			if (getDistance(objPosition, position) < WINDOW_WIDTH / 2 && abs(position.x - objPosition.x) < WINDOW_WIDTH / 2)
			{
				((Waver*)object)->active(position.x > objPosition.x);
			}
		}

		// Ra xa một khoảng thì deactive
		if (((Waver*)object)->isActive())
		{
			auto objPosition = object->getPosition();
			auto position = this->getPosition();
			if (getDistance(objPosition, position) > 150 && abs(position.x - objPosition.x) > WINDOW_WIDTH / 2)
			{
				((Waver*)object)->deactive();
			}
		}

		if (!((Waver*)object)->isDead() && _protectTime <= 0)
		{
			if (collisionBody->checkCollision(object, direction, dt))
			{
				// Nếu đang va chạm thì dời ra xa
				float moveX, moveY;
				if (collisionBody->isColliding(object, moveX, moveY, dt))
				{
					collisionBody->updateTargetPosition(object, direction, false, GVector2(moveX, moveY));
				}

				beHit(direction);
				_info->setEnergy(_info->getEnergy() - 8);
			}

			eID weaponId;
			if (this->checkWeaponCollision(object, direction, weaponId, dt))
			{
				if (weaponId == eID::MISSILE_ROCKET)
					((Waver*)object)->wasHit(5);
				else
					((Waver*)object)->wasHit(1);
			}
		}
	}
	else if (objectId == SKREE)
	{
		// Lại gần thì active
		if (!((Skree*)object)->isActive())
		{
			auto objPosition = object->getPosition();
			auto position = this->getPosition();
			if (abs(position.x - objPosition.x) < 40)
			{
				((Skree*)object)->active();
			}
		}

		if (!((Skree*)object)->isDead() && _protectTime <= 0)
		{
			if (collisionBody->checkCollision(object, direction, dt, false))
			{
				// Nếu đang va chạm thì dời ra xa
				float moveX, moveY;
				if (collisionBody->isColliding(object, moveX, moveY, dt))
				{
					collisionBody->updateTargetPosition(object, direction, false, GVector2(moveX, moveY));
				}

				beHit(direction);
				_info->setEnergy(_info->getEnergy() - 8);
			}

			eID weaponId;
			if (this->checkWeaponCollision(object, direction, weaponId, dt))
			{
				if (weaponId == eID::MISSILE_ROCKET)
					((Skree*)object)->wasHit(5);
				else
					((Skree*)object)->wasHit(1);
			}
		}
	}
	else if (objectId == MELLOW)
	{
		// Lại gần thì active
		if (!((Mellow*)object)->isActive())
		{
			auto objPosition = object->getPosition();
			auto position = this->getPosition();
			if (getDistance(objPosition, position) < WINDOW_WIDTH / 2 && abs(position.x - objPosition.x) < WINDOW_WIDTH / 2)
			{
				((Mellow*)object)->active(position.x > objPosition.x);
			}
		}

		// Ra xa một khoảng thì deactive
		if (((Mellow*)object)->isActive())
		{
			auto objPosition = object->getPosition();
			auto position = this->getPosition();
			if (getDistance(objPosition, position) > 150 && abs(position.x - objPosition.x) > WINDOW_WIDTH / 2)
			{
				((Mellow*)object)->deactive();
			}
		}

		if (!((Mellow*)object)->isDead() && _protectTime <= 0)
		{
			if (collisionBody->checkCollision(object, direction, dt))
			{
				// Nếu đang va chạm thì dời ra xa
				float moveX, moveY;
				if (collisionBody->isColliding(object, moveX, moveY, dt))
				{
					collisionBody->updateTargetPosition(object, direction, false, GVector2(moveX, moveY));
				}

				beHit(direction);
				_info->setEnergy(_info->getEnergy() - 8);
			}

			eID weaponId;
			if (this->checkWeaponCollision(object, direction, weaponId, dt))
			{
				if (weaponId == eID::MISSILE_ROCKET)
					((Mellow*)object)->wasHit(5);
				else
					((Mellow*)object)->wasHit(1);
			}
		}
	}
	else if (objectId == ENERGY_BALL)
	{
		if (collisionBody->checkCollision(object, direction, dt, false))
		{
			_info->setEnergy(_info->getEnergy() + 5);
			object->setStatus(DESTROY);
		}
	}
	else if (objectId == MARU_MARI)
	{
		if (collisionBody->checkCollision(object, direction, dt, false))
		{
			_info->setMaruMari(true);
			object->setStatus(DESTROY);
		}
	}
	else if (objectId == LONG_BEAM)
	{
		if (collisionBody->checkCollision(object, direction, dt, false))
		{
			_info->setBulletRange(_info->getBulletRange() * 2);
			object->setStatus(DESTROY);
		}
	}
	else if (objectId == ICE_BEAM)
	{
		if (collisionBody->checkCollision(object, direction, dt, false))
		{
			_info->setBullet(eID::ICE_BULLET);

			// Nếu weapon đang là Bullet thì chuyển sang Ice Bullet
			// Còn nếu đang là Rocket hoặc Bomb thì không chuyển
			if (_info->getWeapon() != eID::MISSILE_ROCKET && _info->getWeapon() != eID::BOMB)
				_info->setWeapon(_info->getBullet());

			object->setStatus(DESTROY);
		}
	}
	else if (objectId == MISSILE_ROCKET_BALL)
	{
		if (collisionBody->checkCollision(object, direction, dt, false))
		{
			_info->setMissileRocket(_info->getMissileRocket() + 5);
			object->setStatus(DESTROY);
		}
	}
	else if (objectId == BOMB_BALL)
	{
		if (collisionBody->checkCollision(object, direction, dt, false))
		{
			_info->setBomb(true);
			object->setStatus(DESTROY);
		}
	}
}

void Player::setStatus(eStatus status)
{
	_status = status;
}

GVector2 Player::getPosition()
{
	return _sprite->getPosition();
}

int Player::getLifeNumber()
{
	return _info->getLife();
}

RECT Player::getBounding()
{
	RECT bound = _sprite->getBounding();
	
	if ((_currentAnimateIndex & LOOKING_UP) == LOOKING_UP)
	{
		bound.top -= 6;
	}

	if ((_currentAnimateIndex & NORMAL) == NORMAL)
	{
		if (this->getScale().x > 0)
		{
			bound.right -= 3;
			bound.left += 3;
		}
		else if (this->getScale().x < 0)
		{
			bound.left += 3;
			bound.right -= 3;
		}
	}

	return bound;
}

GVector2 Player::getVelocity()
{
	auto movement = (Movement*)this->_componentList["Movement"];
	return movement->getVelocity();
}
