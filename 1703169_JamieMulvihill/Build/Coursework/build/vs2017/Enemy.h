#pragma once
#include "game_object.h"
#include "Player.h"
#include <list>
class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();

	///Collision Response function
	void CollisionResponse(const b2Body * other_body_, GameObject * gameObject);

	///getters and seters for enemy ireator posion
	std::list<Enemy*>::iterator GetIterator() { return enemyIter; }
	void SetIterator(std::list<Enemy*>::iterator iter) { enemyIter = iter; }

private:
	///private variables
	///list iterator used so that on a collision the specific enemy can be deleted
	std::list<Enemy*>::iterator enemyIter;
};

