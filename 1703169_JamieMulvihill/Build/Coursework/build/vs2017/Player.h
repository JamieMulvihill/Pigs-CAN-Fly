#pragma once
#include "game_object.h"
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <graphics\sprite_renderer.h>
#include <graphics/sprite.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <graphics/renderer_3d.h>
#include <audio/audio_manager.h>

class Player : public GameObject
{
public:
	/// Player State enum variable used to check what the player is currently doing
	enum PLAYERSTATE {STATIC, LAUNCHED, BOMB, BOOST, SPLIT, DESTROY };
	Player();
	~Player();

	void Update(float frame_time);
	void CollisionResponse(const b2Body* body, GameObject * gameObject) override;
	void Render(gef::SpriteRenderer* sprite_renderer_);
	void HandleInput(const gef::SonyController* controller, gef::InputManager * i, float frame_time, gef::AudioManager* audio_manager_, gef::Platform* platform_, gef::VolumeInfo);

	/// function to convert from world to screen postion
	void WorldtoScreenConversion(const gef::Matrix44& projection, const gef::Matrix44& view, gef::Vector4& ScreenPosition, float width, float height, float pan);

	///getters and setters for layer bools
	bool getHitEnemy() { return hitEnemy; }
	void setHitEnemy(bool hitStatus) { hitEnemy = hitStatus; }

	bool getHitObstacle() { return hitObstacle; }
	void setHitObstacle(bool hitStatus) { hitObstacle = hitStatus; }

	bool getLaunched() { return isLaunched; }
	void setLaunched(bool launched) { isLaunched = launched; }

	bool getIsSelected() { return isSelected; }
	void setIsSelected(bool selected) { isSelected = selected; }

	bool getIsLaunched() { return isLaunched; }
	void setIsLaunched(bool launched) { isLaunched = launched; }

	bool getIsValid() { return isValid; }
	void setIsValid(bool valid) { isValid = valid; }

	/// getter and setter for the spawn posion
	gef::Vector4 getSpawnPosition() { return spawn; }
	void setSpawnPosition(gef::Vector4 spawnPosition) { spawn = spawnPosition; }

	///getter and setter for players force
	b2Vec2 getForce() { return force; }
	void setForce(b2Vec2 force_) { force = force_; }

	///getter for the players hit gameObject
	GameObject * getCollidedObject() { return hitObj; }

	float getVelovity() { return velocity; }

	///Player state enum variable
	PLAYERSTATE state_;
	/// vector 4 variables used to convert from world to screen position
	gef::Vector4 screenPosition;
	gef::Vector4 spawn;
	
private:
	/// priavte functions
	void BoostGamePlay(int frameSpeed);
	void BombGamePlay(float frame_time, int frameSpeed);
	float Lerp(float start, float end, float time);
	/// function to create a sprite animation, takes in a sprite array, size of the array, name of the files, position, height and width
	void CreateSpriteAnimation(gef::Sprite* array, int size, const char* name, gef::Vector4 position, int height, int width, gef::Texture * texture);
	
	float fps_, hitBoxSize;
	
	/// variables used to create sounds
	Int32 explode_sfx_id, fart_sfx_id;

	///bool variables used
	bool isLaunched;
	bool isSelected;
	bool isBoosting;
	bool isBombing;
	bool scored;
	bool gotPosition;
	bool isValid;
	bool hitEnemy, hitObstacle;
	bool isAlive;

	///pointer to a gameobject, this is used to store what exactly the player has hit in collsions
	GameObject * hitObj;

	///variables for storing velocity and force
	float velocity;
	b2Vec2 force;

	///variables used for sprite animation, array of sprites, a texture*, a bool and a int to get current sprite count
	gef::Sprite explosionSprites[12];
	int explosionFrameAnimCount, explosionCounter;
	gef::Texture* explosion;
	bool hasExploded = false;

	///variables used for sprite animation, array of sprites, a texture*, a bool and a int to get current sprite count
	gef::Sprite boostSprites[10];
	int spriteCount, spriteCounter;
	gef::Texture* boostWhoosh;
	bool hasBoosted = false;

	int frameSpeed;

	gef::Platform* platform_;
	int scale;
};

