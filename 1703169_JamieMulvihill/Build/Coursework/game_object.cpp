#include "game_object.h"
#include <system/debug_log.h>
#include <iostream>


/// UpdateFromSimulation
/// Update the transform of this object from a physics rigid body
void GameObject::UpdateFromSimulation()
{
	if (body_)
	{
		/// setup object rotation
		gef::Matrix44 object_rotation;
		object_rotation.RotationZ(body_->GetAngle());

		/// setup the object translation
		gef::Vector4 object_translation(body_->GetPosition().x, body_->GetPosition().y, 0.f);

		/// build object transformation matrix
		gef::Matrix44 object_transform = object_rotation;
		object_transform.SetTranslation(object_translation);
		set_transform(object_transform);
	}
}
/// function to initialize the gameObjects body
void GameObject::initBody(const gef::Mesh *mesh, b2BodyType btype_, b2Shape::Type bshape_, b2Vec2 position, b2World * world_, b2Vec2 halfsize, float radius, float density, uint16 categoryBits, uint16 maskBits) {
	
	/// setup the mesh for the player
	set_mesh(mesh);

	/// create a physics body for the player
	b2BodyDef body_def;
	body_def.type = btype_;
	body_def.position = position;

	body_ = world_->CreateBody(&body_def);
	body_->SetUserData(this);

	/// create a polygon shape for the player
	if (bshape_ == b2Shape::e_polygon) {
		b2PolygonShape player_shape;
		player_shape.SetAsBox(halfsize.x, halfsize.y);

		b2FixtureDef player_fixture_def;
		player_fixture_def.shape = &player_shape;
		player_fixture_def.density = density;

		player_fixture_def.filter.categoryBits = categoryBits;
		player_fixture_def.filter.maskBits = maskBits;

		body_->CreateFixture(&player_fixture_def);
	}

	/// create a cirecle shape
	else if (bshape_ == b2Shape::e_circle) {
		b2CircleShape  player_shape;
		player_shape.m_p.Set(0,0);
		player_shape.m_radius = radius;

		b2FixtureDef player_fixture_def;
		player_fixture_def.shape = &player_shape;
		player_fixture_def.density = density;
		player_fixture_def.friction = .4;

		player_fixture_def.filter.categoryBits = categoryBits;
		player_fixture_def.filter.maskBits = maskBits;

		body_->CreateFixture(&player_fixture_def);
	}

	
	UpdateFromSimulation();
}

/// function destroy gameobjects, calls the world to detroy bosy and sets to null
void GameObject::CleanUp(b2World * world_)
{
	world_->DestroyBody(body_);
	body_ = NULL;
}
