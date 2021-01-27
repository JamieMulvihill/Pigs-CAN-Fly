#include "Player.h"
#include <system/debug_log.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "load_texture.h"
#include <cmath>


Player::Player() :
	isSelected(false),
	isLaunched(false),
	isBoosting(false),
	scored(false),
	gotPosition(false),
	isValid(false),
	hitEnemy(false),
	hitObstacle(false),
	isBombing(false),
	explode_sfx_id(-1),
	fart_sfx_id(-1),
	state_(STATIC),
	force(0, 0),
	scale(8),
	hitBoxSize(.5),
	frameSpeed(2)
{
	
	type_ = PLAYER;

	explosionFrameAnimCount = 0;
	explosionCounter = 0;

	spriteCount = 0;
	spriteCounter = 0;

	spawn.set_x(0);
	spawn.set_y(0);
	spawn.set_z(0);
	spawn.set_w(0);

	screenPosition = gef::Vector4(0, 0, 0, 0);
	setAlive(true);

	CreateSpriteAnimation(explosionSprites, 11, "Animations/ExplodeAnim/tile0", spawn, 128, 128, explosion);

	CreateSpriteAnimation(boostSprites, 10, "Animations/SmokeAnim/Smoke0", spawn, 64, 64, boostWhoosh);
}

Player::~Player()
{
}

void Player::Update(float frame_time){

	/// call gameplay functions
	BombGamePlay(frame_time, frameSpeed);
	BoostGamePlay(frameSpeed);

	/// sets the velocity of the player
	velocity = std::sqrt(pow(body_->GetLinearVelocity().x, 2) + pow(body_->GetLinearVelocity().y, 2));

	if (getForce().x > 300) {
		setForce(b2Vec2(300, getForce().y));
	}
} 

///collision response function, checks the gameobject of what the player has collided with
void Player::CollisionResponse(const b2Body * body, GameObject * gameObject){

	if (gameObject->type_ == GameObject::ENEMY ) {
		setHitEnemy(true);
		hitObj = gameObject;
	}
	else if (gameObject->type_ == GameObject::OBSTACLE) {
		setHitObstacle(true);
		hitObj = gameObject;
	}
}

/// function to handle the players input, if the player has launched, the player is able to take input
/// based on a button input change state of the player, play its sound, set its relveant bool and call its function 
/// Circle button used to abrt current player and spawn a new one
void Player::HandleInput(const gef::SonyController* controller, gef::InputManager * i,float frame_time, gef::AudioManager* audio_manager_, gef::Platform* platform_, gef::VolumeInfo volInfo ){

	if (getLaunched()) {
		if (state_ == LAUNCHED) {
			if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE) {
				///bomb state functiontionality
				state_ = BOMB;
				explode_sfx_id = audio_manager_->LoadSample("GameSFX/ExplodeSound.wav", *platform_);
				audio_manager_->SetSampleVoiceVolumeInfo(explode_sfx_id, volInfo);
				audio_manager_->PlaySample(explode_sfx_id);
				hasExploded = true; /// bool used for cyclying through animation
				isBombing = true; 
			}
			else if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE) {
				///boost state functionality
				state_ = BOOST;
				fart_sfx_id = audio_manager_->LoadSample("GameSFX/FartEffect.wav", *platform_);
				audio_manager_->SetSampleVoiceVolumeInfo(fart_sfx_id, volInfo);
				audio_manager_->PlaySample(fart_sfx_id);
				isBoosting = true;
			}
		}
		/// set player Alive to false, this is used to spawn new player
		if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE) {
			setAlive(false);
		}
	}
}

/// function for the players boost gameplay functionality, if boosting bool is true and  boost animation bool is not true, increase the force in the x axis and lower the y value, 
/// set the animation bool to true, set the sprites postion and rotaion,increase the values that are used for the animation frame that is being rendered
void Player::BoostGamePlay(int frameSpeed) {

	if (isBoosting && !hasBoosted) {
		force.x *= 1.3;
		force.y *= -1.5;
		body_->ApplyForceToCenter(force, true);
		hasBoosted = true;
	}

	if (hasBoosted) {
		for (gef::Sprite &s : boostSprites) {
			s.set_position(gef::Vector4(spawn.x() - 45, spawn.y() - 10, 0));
			s.set_rotation(275);
		}

		if (spriteCount < 9) {
			spriteCounter++;
			if (spriteCounter % frameSpeed == 0) {
				spriteCount++;
			}
		}

		else {
			isBoosting = false;
			hasBoosted = false;
			spriteCounter = 0;
			spriteCount = 0;
		}
	}
}

/// function for the players Bomb gameplay functionality, if bombing bool is true, increase the radius of the players body and set the players velocity to 0
/// set the animations positions to be the players, set postion bool to true, check if the explosion animation bool has been sett to true,
/// increase the values that are used for the animation frame that is being rendered and set player alive to false
void Player::BombGamePlay(float frame_time, int frameSpeed) {

	if (isBombing) {
		if (hitBoxSize < 4) {
			hitBoxSize += (5 * frame_time);
			float i = hitBoxSize;
			if (this->state_ == BOMB) {
				body_->SetLinearVelocity(b2Vec2(0, 0));
				fixturePtr = body_->GetFixtureList();
				shapePtr = fixturePtr->GetShape();
				shapePtr->m_radius = i;
				set_mesh(NULL);
			}

			if (hasExploded && !gotPosition) {

				for (gef::Sprite &spriteAnim : explosionSprites) {
					spriteAnim.set_position(spawn);
				}

				gotPosition = true;
			}

			if (hasExploded) {
				if (explosionFrameAnimCount < 11) {
					explosionCounter++;
					if (explosionCounter % frameSpeed == 0) {
						explosionFrameAnimCount++;
					}
				}
				else {
					hasExploded = false;
					explosionCounter = 0;
					explosionFrameAnimCount = 0;
					setAlive(false);
				}
			}
		}
		else {
			isAlive = false;
		}
	}
	
}

float Player::Lerp(float start, float end, float time)
{
	float lerpValue = start + (end - start) * time;
	return lerpValue;
}

/// function to create animantion of sprites, uses the size int parameter to loop though and populate an array of sprites, converts the i value from an int to string, 
/// adds this string to the name char* parameter, adds ".png" to name string, intializes a texture with name string, assigns texture to sprite in array at i index
void Player::CreateSpriteAnimation(gef::Sprite * array, int size, const char * name, gef::Vector4 position, int height, int width, gef::Texture * texture){

	for (int i = 0; i < size; i++) {
		array[i].set_position(position);
		array[i].set_width(height);
		array[i].set_height(width);

		int value = i;
		std::string string;
		std::stringstream out;
		out << value;
		string = out.str();

		std::string num = string;
		std::string temp = name;
		std::string extendeer = ".png";
		temp += num;
		temp += extendeer;
		texture = CreateTextureFromPNG(temp.c_str(), *platform_);
		array[i].set_texture(texture);
	}
}

void Player::WorldtoScreenConversion(const gef::Matrix44 & projection, const gef::Matrix44& view, gef::Vector4& ScreenPosition, float width, float height, float pan)
{
	gef::Matrix44 vp = view * projection;

	/// convert world space position (spawn) to ndc
	gef::Vector4 pp = transform_.GetTranslation() ;
	pp.set_w(1.f);
	pp = pp.TransformW(vp);

	/// convert ndc to screen position
	pp = pp / pp.w();
	ScreenPosition = gef::Vector4((pp.x() + 1) / 2.f * width, (1 - pp.y()) / 2.f * height, -0.9f, 0.0f);
}

///function to render the sprite animations based on player bool values
void Player::Render(gef::SpriteRenderer* sprite_renderer_) {

	if (hasExploded && gotPosition) {
		sprite_renderer_->DrawSprite(explosionSprites[explosionFrameAnimCount]);
	}
	if (hasBoosted) {
		sprite_renderer_->DrawSprite(boostSprites[spriteCount]);
	}
	
}