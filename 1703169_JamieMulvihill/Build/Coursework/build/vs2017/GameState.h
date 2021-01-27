#pragma once
#include <system/application.h>
#include <maths/vector2.h>
#include <cstdlib>
#include "Player.h"
#include "EnemyManager.h"
#include "ObstacleManager.h"
#include "Enemy.h"
#include "Obstacle.h"
#include "graphics/scene.h"
#include "State.h"
#include "Obstacle.h"
#include "MenuState.h"
#include "WinState.h"
#include "LoseState.h"
#include <maths/math_utils.h>
#include "GUI.h"
#include <thread>
#include "Asset.h"

class GameState : public State
{
public:

	GameState(gef::Platform& p);
	~GameState();

	///functions to initialize, render, update and delete 
	void Init(gef::InputManager* input_manager, gef::AudioManager* audio_manager);
	void Render(gef::SpriteRenderer * spriteRenderer, float width, float height, gef::Font * font);
	void Update(float frame_time, gef::InputManager * inputManager, State ** nextState);
	void Release(gef::AudioManager* audio_manager);

private:

	/// functions to split the initialize time whilst load screen is being rendered
	void LoadFunction(); /// starts the load process by creating world and initializing sounds
	void SetUpLoadFunction(gef::InputManager * input_manager); /// function to initialize game set up, i.e. input manager, 3drenederer, gui, font and difficulty
	void ObjectsLoadFunction(); /// function to isitialize game assets

	void SetUpLights();

	/// functions to read the user pressing the screen and checking against the active player position
	void GetScreenPosRay(const gef::Vector2 & screen_position, const gef::Matrix44 & projection, const gef::Matrix44 & view, gef::Vector4 & startPoint, gef::Vector4 & direction, float width, float height);
	bool RaySphereIntersect(gef::Vector4 & startPoint, gef::Vector4 & direction, gef::Vector4 & sphere_centre, float sphere_radius);
	/// function to update the sprites position and rotataion, check if user input is valid and play sounds
	bool Selection(gef::InputManager * input_manager, gef::Renderer3D * renderer, float width, float height);

	///function to launch the active player
	void Launch();
	void HandleInput(gef::InputManager* input_manager, float frame_time);
	void DrawHUD(gef::SpriteRenderer * spriteRenderer);
	void CollisionDetection();
	void UpdatePhysicsBodies();
	void CleanUpFont();

	///function to remove objects in the game
	void RemovingObjects();
	int SetDifficulty(State::DIFFICULTY difficulty);
	void DeleteParticales(std::vector<GameObject *>* vector);
	///function to call all the releveant functions for the game to run 
	void GamePlay(gef::InputManager* input_manager_);
	///function to spawn debris when a player hits an obstacle, takes a pointer to the obstalce that the player has hit as a parameter to get spawn postion for debris
	void SpawnDebris(Obstacle * plank);
	///function for spawning new player
	void NewPlayer();
	void AddScore();
	void Pan(gef::InputManager* input_manager);

	///functions used to initialze and create objects in level whilst lad screen is rendering
	void InitObjects();/// function to initialize all the assets in game, as the number of assets is high, function utilises multithreading to work faster
	void InitSounds(gef::AudioManager * audio_manager);
	void InitFont();
	void CheckLevelWinorLose();

	/// int values to set the time to call each function during load screen
	int loadFirstFunction, loadSecondFunction, loadThirdFunction;

	/// difficulty variable to set difficulty
	DIFFICULTY difficulty;
	
	/// Asset instances used in game
	Asset enemyAsset;
	Asset PlayerAsset;
	Asset foregroundAsset;
	Asset middlegroundAsset;
	Asset backgroundAsset;
	Asset woodHorizontalAsset;
	Asset woodVerticalAsset;
	Asset slingAsset;
	Asset woodChipAsset;

	/// ground variables
	gef::Mesh* ground_mesh_;
	GameObject ground_;
	GameObject wall_;
	b2Body* ground_body_;

	/// textures used for displaying loading screen 
	gef::Texture* tex;
	gef::Texture* loadBar;
	gef::Texture* loadAmount;
	gef::Texture* loadText;
	gef::Texture * background;
	
	///instance of gui class
	GUI gui;

	gef::MeshInstance mesh_SlingShot;
	GameObject start_;

	/// instances of the EnemyManger and Obstacle manager to control in game
	EnemyManager enemyManager;
	ObstacleManager obstacleManager;
	
	gef::Font* font_;
	gef::Renderer3D* renderer_3d_;

	/// create the physics world
	b2World* world_;

	std::vector<GameObject *> debrisBits;

	/// player variables
	Player player_;
	int maxPlayers;
	std::vector<Player *> players;
	Player * activePlayer;

	/// variables used for getting the user touch input and calculating pan
	Int32 active_touch_id_;
	float pan;
	gef::Vector2 start;
	gef::Vector2 current;
	gef::Vector4 screenPosition;
	float cameraX, magnitude;

	///varibles user for creating slingshot sprite 
	float ndc_z_min_, activeMag;
	gef::Vector2 touch_position_, release_position_, distance;
	gef::Sprite slingshotBand;

	float fps_;
	
	/// bool values used in game
	bool isTouched, levelWon, levelLost, isLoading, isLaunchable;

	int playerCount;

	int scoreMultiplier;

	/// variables used for creating sound effects 
	Int32 selected_sfx_id, launched_sfx_id, squeal_sfx_id, break_sfx_id, score_sfx_id;

	///variales used for adding score
	int score, newScore;

	///variables used for load screen calculations
	int loadCounter;
	bool gameStart;

	/// vector of threads used to load assets in game
	std::vector<std::thread > assetThreads;
};

