#include "Enemy.h"
#include <system/debug_log.h>
#include <iostream>

Enemy::Enemy()
{
	type_ = ENEMY;
}


Enemy::~Enemy()
{
}

void Enemy::CollisionResponse(const b2Body * other_body_, GameObject * gameObject)
{
	///if the Player collides wih an enemy set enemy alive to false
	if (gameObject->type_ == GameObject::PLAYER) {
		setAlive(false);
	}
}

