#include "Obstacle.h"

Obstacle::Obstacle(): 
	isAlive(true),
	isHit(false)
{
	type_ = OBSTACLE;
	force.x = 500;
	force.y = 500;
	health = 50;
}


Obstacle::~Obstacle(){
}

/// collision response function, takes a gameobject* as parameter, Checks if the gameObject* has a type equal player,
/// if it does equal player, cast gameObject* to player, if player has a state equl to bomb,
/// apply forces relative to postion of player, decrease health of obstacle,
/// if player state is not bomb, decrease health according to players velocity,
/// if health is zero or lower set bools to isHit and not is alive 
void Obstacle::CollisionResponse(const b2Body * other_body_, GameObject * gameObject){

	if (gameObject->type_ == GameObject::PLAYER) {

		
		Player * player_ = (Player*)gameObject;

		if (player_->state_ == Player::BOMB) {
			if (body_->GetPosition().x < player_->getBody()->GetPosition().x) {
				force.x = -1*(force.x);
				health -= 40;
			}
			else if(body_->GetPosition().x > player_->getBody()->GetPosition().x) {
				force.x = (force.x);
				health -= 40;
			}
			if (body_->GetPosition().y < player_->getBody()->GetPosition().y) {
				force.y = -1 * (force.y);
				health -= 40;
			}
			else if (body_->GetPosition().y > player_->getBody()->GetPosition().y) {
				force.y = (force.y);
				health -= 40;
			}
			body_->ApplyForceToCenter(force, true);
		}
		else {
			health -= (player_->getVelovity() * 2);

			if (health <= 0) {
				isAlive = false;
				isHit = true;
			}
		}
	}
}
