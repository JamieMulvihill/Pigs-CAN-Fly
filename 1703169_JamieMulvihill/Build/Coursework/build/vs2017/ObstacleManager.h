#pragma once
#include "Obstacle.h"
#include <graphics/renderer_3d.h>
#include <box2d/Box2D.h>
#include <list>
class ObstacleManager
{
public:
	~ObstacleManager();

	///functions to initialize, render, Delete and update the obstacles in the list
	void InitObstacles(const gef::Mesh * meshH, const gef::Mesh * meshV, b2BodyType btype_, b2Shape::Type bshape_, b2World * world_, b2Vec2 halfsizeH, b2Vec2 halfsizeV, float radius, float density, uint16 categoryBits, uint16 maskBits, const char * filename);
	void Render(gef::Renderer3D* renderer);
	void DeleteObstacle(b2World* world_, Obstacle * obstacle);
	void UpdatePhysicsBodies();

	std::list<Obstacle*> obstacles;
private:
	///function to read in values from a file to set obstacle positons
	char * FileReader(const char * filename);
	void LoadObstaclesFromFile(const char * filename);

	///vector of B2Vec2 to store obstacle positions
	std::vector<b2Vec2> obstaclePositions;
	
};

