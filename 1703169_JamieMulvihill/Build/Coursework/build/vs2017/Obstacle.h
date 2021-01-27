#pragma once
#include "game_object.h"
#include "Player.h"
class Obstacle : public GameObject
{
public:

	///enum of obstable orientation 
	enum OBSTACLESTATE { HORIZONTAL, VERTICAL };

	Obstacle();
	~Obstacle();

	/// obstaclestate enum variable
	OBSTACLESTATE state_;
	void CollisionResponse(const b2Body * other_body_, GameObject * gameObject) override;
	
	/// getters and setters for list iterator
	std::list<Obstacle*>::iterator GetIterator() { return obstacleIter; }
	void SetIterator(std::list<Obstacle*>::iterator iter) { obstacleIter = iter; }

private:

	/// each obstacle is given a list iterator so player can acess its postion in list and delete withou looping
	std::list<Obstacle*>::iterator obstacleIter;
	b2Vec2 force;
	bool isAlive, isHit;
	int health;
};

