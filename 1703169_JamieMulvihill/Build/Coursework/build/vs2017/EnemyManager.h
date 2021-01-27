#pragma once
#include "Enemy.h"
#include <graphics/renderer_3d.h>
#include <box2d/Box2D.h>
#include <list>

class EnemyManager
{
public:
	~EnemyManager();

	///functions to initialize, render, Delete and update the enemies in the list
	void InitEnemies(const gef::Mesh *mesh, b2BodyType btype_, b2Shape::Type bshape_, b2World * world_, b2Vec2 halfsize, float radius, float density, uint16 categoryBits, uint16 maskBits, const char* filename);
	void Render(gef::Renderer3D* renderer);
	void DeleteEnemy(b2World* world_, Enemy * enemy);
	void UpdatePhysicsBodies();

	std::list<Enemy*> enemies;
private:
	///function to read in values from a file to set enemy positons
	char * FileReader(const char * filename);
	void LoadEnemiesFromFile(const char* filename);

	///vector of B2Vec2 to store enemy positions
	std::vector<b2Vec2> enemyPositions;
	
};

