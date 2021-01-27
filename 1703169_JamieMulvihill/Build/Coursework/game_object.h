#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <system/debug_log.h>
#include <graphics/mesh_instance.h>
#include <box2d/Box2D.h>

class GameObject : public gef::MeshInstance
{
public:

	/// enums for type of gameObject, and collsion masks
	enum ObjectType {PLAYER, ENEMY, BULLET, GROUND, OBSTACLE, PARTICLE};
	enum CollisionCategory {ALWAYSCOLLIDE = 0x0001, SOMETIMESCOLLIDE = 0x0002, NEVERCOLLIDE = 0x0004};

	/// functions to be overwritten by child classes
	void UpdateFromSimulation();
	virtual void CollisionResponse(const b2Body* body, GameObject * gameObject) {};
	void CleanUp(b2World* world_);
	void initBody(const gef::Mesh *mesh, b2BodyType btype_, b2Shape::Type bshape_, b2Vec2 position, b2World * world_, b2Vec2 halfsize, float radius, float density, uint16 categoryBits, uint16 maskBits);

	///getters and setters for bool isAlive
	bool getAlive() { return isAlive; }
	void setAlive(bool alive) { isAlive = alive; }

	///getters and setters for b2bodt body
	b2Body* getBody() { return body_; }
	void setBody(b2Body* body) { body_ = body; }

	ObjectType type_;
	b2Fixture *fixturePtr;
	b2Shape *shapePtr;

protected:
	bool isAlive;
	b2Body* body_;
	
};

#endif // _GAME_OBJECT_H