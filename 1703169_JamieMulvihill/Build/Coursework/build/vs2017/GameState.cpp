#include "GameState.h"
#include "scene_app.h"
#include <system/debug_log.h>
#include <iterator>

///initializer list for gamestate, sets all variable values used in state
GameState::GameState(gef::Platform& p) :
	State(p),
	renderer_3d_(NULL),
	world_(NULL),
	active_touch_id_(-1),
	selected_sfx_id(-1),
	squeal_sfx_id(-1),
	break_sfx_id(-1),
	score_sfx_id(-1),
	levelWon(false),
	isLaunchable(false),
	levelLost(false),
	magnitude(0),
	maxPlayers(0),
	playerCount(0),
	pan(0),
	cameraX(0),
	score(0),
	ndc_z_min_(-1.0f),
	newScore(1000),
	activeMag(0),
	loadCounter(0),
	loadFirstFunction(80),
	loadSecondFunction(160),
	loadThirdFunction(350),
	scoreMultiplier(0),
	gameStart(false),
	isLoading(false)
{
	state_ = PLAYLEVEL;
}

GameState::~GameState()
{
}

///Camera Lerping code modified from Peter Hall Git Repository
///https://gitlab.com/cmp208-2018/polishing_a_game/commit/5b6037d044d3b78b8ced4eac452b85b2904e35c9
struct Camera
{
	float time_;
	float translate_;
	float rotate_;
	float pan_;

	gef::Vector4 pos_;
	gef::Vector4 camera_eye_;

	Camera()
	{
		camera_eye_ = gef::Vector4(15 - pan_, 10, 10);
	}

	void Shake(float time, float translate, float rotate)
	{
		time_ = time;
		translate_ = translate;
		rotate_ = rotate;
	}

	float sax, say, saz;

	void Update(float frame_time)
	{
#define SHAKESPEEDX 40.0f
#define SHAKESPEEDY 30.0f
#define SHAKESPEEDZ 10.0f

		if (time_ >= 0.f)
		{
			time_ -= frame_time;
			sax += SHAKESPEEDX * frame_time;
			say += SHAKESPEEDY * frame_time;
			saz += SHAKESPEEDZ * frame_time;
		}
	}

	void Render(gef::Matrix44 &view_matrix, float x, float y)
	{
		camera_eye_.Lerp(camera_eye_, gef::Vector4(x, y + 6.0f + x / 10.f, 45.0f - (x / 5.f)), 0.05f);

		gef::Vector4 camera_lookat((x - 2) - pan_, y, 0.0f);
		gef::Vector4 camera_up(0.0f, 1.0f, 0.0f);

		if (time_ > 0.f)
		{
			float shakex = time_ * sinf(sax);
			float shakey = time_ * sinf(say);
			float shakez = time_ * sinf(saz);

			camera_lookat.set_x(camera_lookat.x() + translate_ * shakex);
			camera_lookat.set_y(camera_lookat.y() + translate_ * shakey);
			camera_lookat.set_z(camera_lookat.z() + translate_ * shakez);

			camera_up.set_x(camera_up.x() + rotate_ * shakex);
			camera_up.set_y(camera_up.y() + rotate_ * shakey);
			camera_up.set_z(camera_up.z() + rotate_ * shakez);
		}

		view_matrix.LookAt(camera_eye_, camera_lookat, camera_up);
	}
};

Camera camera_;

float randRange(float min, float max)
{
	float r = (float)rand() / (float)RAND_MAX;
	r = (r * (max - min)) + min;
	return r;
}


///function to initilze the loading screen textures and audio manager
void GameState::Init(gef::InputManager* input_manager_, gef::AudioManager* audio_manager)
{
		loadBar = CreateTextureFromPNG("Textures/LoadBar.png", platform_);
		loadAmount = CreateTextureFromPNG("Textures/LoadAmount.png", platform_);
		loadText = CreateTextureFromPNG("Textures/LoadText.png", platform_);
		background = CreateTextureFromPNG("Textures/NewControls.png", platform_);
		
		audio_manager_ = audio_manager;
		
}
void GameState::Update(float frame_time, gef::InputManager * input_manager, State ** nextState){
	fps_ = 1.0f / frame_time;

	///if the game has not started, run a counter and call the load function
	loadCounter++;
	if (!gameStart) {
		LoadFunction();
		SetUpLoadFunction(input_manager);
		ObjectsLoadFunction();
		if (loadCounter > 400) {
			gameStart = true;
		}
	}
	
	/// if the game has statred update the nessecary game object
	else if (gameStart) {
		float32 timeStep = 1.0f / 60.0f;

		int32 velocityIterations = 6;
		int32 positionIterations = 2;

		world_->Step(timeStep, velocityIterations, positionIterations);

		UpdatePhysicsBodies();

		CollisionDetection();

		GamePlay(input_manager);

		activePlayer->Update(frame_time);

		camera_.Update(frame_time);

		gui.Update(frame_time);

		Pan(input_manager);

		HandleInput(input_manager, frame_time);

	}
}
void GameState::Render(gef::SpriteRenderer * sprite_renderer, float width, float height, gef::Font * font) {

	///Render game assests if the game has started
	if (gameStart) {
		///Setting up of the Camera
		float fov = gef::DegToRad(45.0f);
		float aspect_ratio = (float)platform_.width() / (float)platform_.height();
		gef::Matrix44 projection_matrix;
		projection_matrix = platform_.PerspectiveProjectionFov(fov, aspect_ratio, 0.5f, 120.0f);
		renderer_3d_->set_projection_matrix(projection_matrix);
		gef::Matrix44 view_matrix;
		b2Vec2 player_pos = activePlayer->getBody()->GetPosition();
		///camera render lerping
		if (activePlayer->getLaunched())
			camera_.Render(view_matrix, player_pos.x, player_pos.y);
		else
			camera_.Render(view_matrix, player_pos.x, player_pos.y);
		renderer_3d_->set_view_matrix(view_matrix);
		///Convert Players co-ordinates to Screen position
		activePlayer->WorldtoScreenConversion(projection_matrix, view_matrix, activePlayer->spawn, width, height, 0);

		/// draw 3d geometry
		renderer_3d_->Begin();

		/// draw meshes
		renderer_3d_->DrawMesh(ground_);
		renderer_3d_->DrawMesh(backgroundAsset.asset_Mesh);
		renderer_3d_->DrawMesh(foregroundAsset.asset_Mesh);
		renderer_3d_->DrawMesh(mesh_SlingShot);
		renderer_3d_->DrawMesh(*activePlayer);

		enemyManager.Render(renderer_3d_);

		obstacleManager.Render(renderer_3d_);
	
		for (GameObject * d : debrisBits) {
			renderer_3d_->DrawMesh(*d);
		}
		renderer_3d_->set_override_material(NULL);
		renderer_3d_->End();

		/// start drawing sprites, but don't clear the frame buffer
		sprite_renderer->Begin(false);

		activePlayer->Render(sprite_renderer);

		if (activePlayer->getIsSelected() && !activePlayer->getLaunched() && activePlayer->getIsValid()) {
			sprite_renderer->DrawSprite(slingshotBand);
		}
		gui.Render(sprite_renderer, width, height, font_, score);
		DrawHUD(sprite_renderer);
		sprite_renderer->End();
	}

	/// If the game has not started, render loading screen
	else {
		sprite_renderer->Begin();

		gef::Sprite backgroundSprite;
		backgroundSprite.set_texture(background);
		backgroundSprite.set_position(gef::Vector4(width*0.50f, height*0.5f, -0.99f));
		backgroundSprite.set_height(height);
		backgroundSprite.set_width(width);
		sprite_renderer->DrawSprite(backgroundSprite);

		gef::Sprite loadBarSprite;
		loadBarSprite.set_texture(loadBar);
		loadBarSprite.set_position(gef::Vector4(width*0.50f, height*0.95f, -0.99f));
		loadBarSprite.set_height(40);
		loadBarSprite.set_width(800);
		sprite_renderer->DrawSprite(loadBarSprite);

		gef::Sprite loadAmountSprite;
		loadAmountSprite.set_texture(loadAmount);
		///to ensure the sprite stays at the correct position on screen its position has to be changed based on the current width minust the the size of the bar it loads within to keep its size rlative
		loadAmountSprite.set_position(gef::Vector4(((width*0.3f + (loadAmountSprite.width()/2) - loadBarSprite.width()/4)), height*0.95f, -0.99f));
		loadAmountSprite.set_height(32);
		/// set width to be the twice the size of the curent load count
		loadAmountSprite.set_width(loadCounter * 2);
		sprite_renderer->DrawSprite(loadAmountSprite);

		gef::Sprite loadTextSprite;
		loadTextSprite.set_texture(loadText);
		loadTextSprite.set_position(gef::Vector4(width*0.5f, height*0.95f, -0.99f));
		loadTextSprite.set_height(24);
		loadTextSprite.set_width(100);
		sprite_renderer->DrawSprite(loadTextSprite);

		font->RenderText(
			sprite_renderer,
			gef::Vector4(width*0.6f, height*0.92f, -0.99f),
			1.0f,
			0xffffffff,
			gef::TJ_CENTRE,
			"... %i %", loadCounter/4);

		sprite_renderer->End();
	}
}
void GameState::Release(gef::AudioManager* audio_manager)
{
	CleanUpFont();

	delete renderer_3d_;
	renderer_3d_ = NULL;

	delete ground_mesh_;
	ground_mesh_ = NULL;

	delete tex;
	tex = NULL;

	delete loadBar;
	loadBar = NULL;

	delete loadAmount;
	loadAmount = NULL;

	delete loadText;
	loadText = NULL;

	delete background;
	background = NULL;

	audio_manager_->UnloadAllSamples();
	audio_manager_->StopMusic();
	audio_manager_->UnloadMusic();

	gui.Release();

	delete world_;
	world_ = NULL;

}

/// fucntoin that starts the load process by creating world and initializing sounds
void GameState::LoadFunction() {

	if (!gameStart) {
		if (loadCounter == loadFirstFunction) {

			InitSounds(audio_manager_);

			/// initialise the physics world
			b2Vec2 gravity(0.0f, -9.81f);
			world_ = new b2World(gravity);

			audio_manager_->PlayMusic();

		}
	}
}
/// function to isitialize game assets
void GameState::ObjectsLoadFunction() {

	if (!gameStart) {
		if (loadCounter == loadSecondFunction) {
			///function to initialize all assets loaded into level and give values to mesh positions etc......
			InitObjects();
		}
	}
}
/// function to initialize game set up, i.e. input manager, 3drenederer, gui, font and difficulty
void GameState::SetUpLoadFunction(gef::InputManager * input_manager) {

	if (!gameStart) {
		if (loadCounter == loadThirdFunction) {

			if (input_manager && input_manager->touch_manager() && (input_manager->touch_manager()->max_num_panels() > 0))
			{
				input_manager->touch_manager()->EnablePanel(0);
			}

			/// create the renderer for draw 3D geometry
			renderer_3d_ = gef::Renderer3D::Create(platform_);
			///function to initialize all assets loaded into level and give values to mesh positions etc.....

			gui.Init(platform_, SetDifficulty(State::getDifficulty()));

			difficulty = State::getDifficulty();

			InitFont();

			SetUpLights();

		}
	}
}
/// function to initialize all the assets in game, as the number of assets is high and all the work is independant, function utilises multithreading to carry out work faster
void GameState::InitObjects(){

	///add a thread to load assets for the player and enemies
	assetThreads.push_back(std::thread([&] {
		
		/// create the mesh for the player and initialize the players starting value
		player_.initBody(PlayerAsset.GetMeshFromSceneAssets(PlayerAsset.LoadSceneAssets(platform_, "Meshes/NewLeggyPig.scn")), b2_dynamicBody, b2Shape::e_circle, b2Vec2(-25, 10), world_, b2Vec2(.5, .5), .5, 1, GameObject::ALWAYSCOLLIDE, GameObject::ALWAYSCOLLIDE);
		players.push_back(&player_);
		players.back()->type_ = GameObject::PLAYER;
		players.back()->setSpawnPosition(gef::Vector4(players.back()->getBody()->GetPosition().x, players.back()->getBody()->GetPosition().y, 0));
		activePlayer = players.back();

	}));
	loadCounter = Lerp(loadCounter, loadCounter += 40, 0.5);

	///add a thread to load assets
	assetThreads.push_back(std::thread([&] {

		enemyManager.InitEnemies(enemyAsset.GetMeshFromSceneAssets(enemyAsset.LoadSceneAssets(platform_, "Meshes/Piggy.scn")), b2_dynamicBody, b2Shape::e_circle, world_, b2Vec2(.5, .5), .5, 5, GameObject::ALWAYSCOLLIDE, GameObject::ALWAYSCOLLIDE, "FileReader/EnemyPostions");

		///create the mesh for obstacles and initialize the obstacle manager
		obstacleManager.InitObstacles(woodHorizontalAsset.GetMeshFromSceneAssets(woodHorizontalAsset.LoadSceneAssets(platform_, "Meshes/WoodHorizontal.scn")) , woodVerticalAsset.GetMeshFromSceneAssets(woodVerticalAsset.LoadSceneAssets(platform_, "Meshes/WoodY.scn")), b2_dynamicBody, b2Shape::e_polygon, world_, b2Vec2(2, .5), b2Vec2(.5, 2), .5, 1, GameObject::ALWAYSCOLLIDE, GameObject::ALWAYSCOLLIDE, "FileReader/ObstaclePositions");
	}));
	

	///add a thread to load assets
	assetThreads.push_back(std::thread([&] {

		///create and load the three meshes that make up the world mesh(foreground, middleground and background)
		ground_.type_ = GameObject::GROUND;
		ground_.initBody(middlegroundAsset.GetMeshFromSceneAssets(middlegroundAsset.LoadSceneAssets(platform_, "Meshes/NewMiddleGround.scn")), b2_staticBody, b2Shape::e_polygon, b2Vec2(0, 0), world_, b2Vec2(80, .5), .5, 1, GameObject::ALWAYSCOLLIDE, GameObject::ALWAYSCOLLIDE);

		gef::Vector4 backGroundMount_translation(0, -.2, -25);
		gef::Matrix44 backGroundMount_transform;
		backGroundMount_transform.SetIdentity();
		backGroundMount_transform.SetTranslation(backGroundMount_translation);
		backgroundAsset.asset_Mesh.set_transform(backGroundMount_transform);
		backgroundAsset.setMesh(backgroundAsset.GetMeshFromSceneAssets(backgroundAsset.LoadSceneAssets(platform_, "Meshes/BGMountain.scn")));
	}));

	///add a thread to load assets
	assetThreads.push_back(std::thread([&] {

		/// create, load and transform the slingshot mesh
		gef::Vector4 slingShot_translation(-25, 1, 0);
		gef::Matrix44 slingShot_transform;
		slingShot_transform.SetIdentity();
		slingShot_transform.SetTranslation(slingShot_translation);
		mesh_SlingShot.set_transform(slingShot_transform);
		mesh_SlingShot.set_mesh(slingAsset.GetMeshFromSceneAssets(slingAsset.LoadSceneAssets(platform_, "Meshes/NewSling.scn")));
		start_.initBody(NULL, b2_staticBody, b2Shape::e_polygon, b2Vec2(slingShot_translation.x(), 4), world_, b2Vec2(1, 5), .5, 1, GameObject::ALWAYSCOLLIDE, GameObject::ALWAYSCOLLIDE);

		tex = CreateTextureFromPNG("Textures/RedTex.png", platform_);
		slingshotBand.set_texture(tex);
		slingshotBand.set_position(gef::Vector4(0, 0, 0));
		slingshotBand.set_width(40);
		slingshotBand.set_height(10);
	}));
	
	///add a thread to load assets
	assetThreads.push_back(std::thread([&] {
		
		woodChipAsset.setMesh(woodChipAsset.GetMeshFromSceneAssets(woodChipAsset.LoadSceneAssets(platform_, "Meshes/WoodChip.scn")));

		wall_.initBody(NULL, b2_staticBody, b2Shape::e_polygon, b2Vec2(55, 30), world_, b2Vec2(1, 30), .5, 1, GameObject::ALWAYSCOLLIDE, GameObject::ALWAYSCOLLIDE);

		foregroundAsset.setMesh(foregroundAsset.GetMeshFromSceneAssets(foregroundAsset.LoadSceneAssets(platform_, "Meshes/Foreground.scn")));

		screenPosition.set_x(activePlayer->getBody()->GetPosition().x);
		screenPosition.set_y(activePlayer->getBody()->GetPosition().y);
	}));
	
	///join threads
	for (std::thread assets : assetThreads) {
		assets.join();
	}

	assetThreads.clear();
	
}
/// function to set the audio manager and add all the sounds
void GameState::InitSounds(gef::AudioManager * audio_manager) {

	selected_sfx_id = audio_manager_->LoadSample("GameSFX/SelectedOink.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(selected_sfx_id, volInfo);

	launched_sfx_id = audio_manager_->LoadSample("GameSFX/LaunchSound.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(selected_sfx_id, volInfo);

	squeal_sfx_id = audio_manager_->LoadSample("GameSFX/PiggySqueal.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(selected_sfx_id, volInfo);

	break_sfx_id = audio_manager_->LoadSample("GameSFX/WoodBreak.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(break_sfx_id, volInfo);

	score_sfx_id = audio_manager_->LoadSample("GameSFX/ScoreSound.wav", platform_);
	audio_manager_->SetSampleVoiceVolumeInfo(score_sfx_id, volInfo);

	audio_manager_->LoadMusic("GameSFX/BackgroundGameMusic.wav", platform_);
}
///function to check the win state, set the score multiplyer based on difficulty and if necessary set lose state
void GameState::CheckLevelWinorLose()
{

	if (enemyManager.enemies.size() < 1) {
		levelWon = true;
	}

	if (levelWon) {
		switch (difficulty) {
		case EASY:
			scoreMultiplier = 10000;
			break;
		case NORMAL:
			scoreMultiplier = 15000;
			break;
		case HARD:
			scoreMultiplier = 20000;
			break;
		}

		SceneApp::SetScore(score + (scoreMultiplier * (maxPlayers - playerCount)));
		SceneApp::SetState(new WinState(platform_));
	}


	if (levelLost) {
		SceneApp::SetState(new LoseState(platform_));
	}

}

void GameState::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}
void GameState::CleanUpFont(){
	delete font_;
	font_ = NULL;
}

///function to remove enemies and obstacles based on collisions
void GameState::RemovingObjects(){

	/// if the active player, hits an obstacle,  return bool hit obstacle as true
	///call the SpawnDrebris funcion, the Spawn Debris function takes an Obstacle* as a paramerter, each player object has a GameObject* called hitObj, the hitObj is then casted to an Obstacle*.
	///call the ObstacleManagers delete function, and pass it the world and active players hitObj
	///set active players hitobstacle bool to false

	if (activePlayer->getHitObstacle()) {
	
		SpawnDebris((Obstacle*)(activePlayer->getCollidedObject()));
		camera_.Shake(0.2f, 0.5f, 0.2f);
		///cast the players hit game object to an obstacle and delete that obstacle
		obstacleManager.DeleteObstacle(world_, (Obstacle*)(activePlayer->getCollidedObject()));
		activePlayer->setHitObstacle(false);
	}

	if (activePlayer->getHitEnemy()) {
		AddScore();
		///cast the players hit game object to an enemy and delete that obstacle
		enemyManager.DeleteEnemy(world_, (Enemy*)(activePlayer->getCollidedObject()));
		activePlayer->setHitEnemy(false);
	}
}
///function to set the number of players(lives) available to the user based on the difficulty seting they have chosen
int GameState::SetDifficulty(State::DIFFICULTY difficulty)
{
	switch (difficulty) {
	case State::EASY:
		maxPlayers = 5;
		break;
	case State::NORMAL:
		maxPlayers = 4;
		break;
	case State::HARD:
		maxPlayers = 3;
	}
	return maxPlayers;
}
///function to delete debris particles, takes a vector* as a parameter so different collections of Gameobjects can be deleteted
void GameState::DeleteParticales(std::vector<GameObject *>* vector)
{
	///creates a gameObject vector iterator
	std::vector<GameObject *>::iterator it;

	/// iteratates trough vector
	for (it = vector->begin(); it != vector->end();)
	{
		///if the gameobejects y position is below a set value, delete it, otherwise continue iterating
		if ((*it)->getBody()->GetPosition().y > -2)
		{
			++it;
		}
		else
		{
			(*it)->CleanUp(world_);
			it = vector->erase(it);
		}
	}
}
///function to call all the releveant functions for the game to run 
void GameState::GamePlay(gef::InputManager* input_manager_){

	///check if the activePlayer has been launched, 
	///if the user has a valid vector drawn using the touch screen(isLaunchable), 
	///call the launch function
	///Selection function is called to check the current active touch of the user
	if (!activePlayer->getLaunched()) {
		if (isLaunchable) {
			Launch();
		}
		Selection(input_manager_, renderer_3d_, platform_.width(), platform_.height());
	}

	///function to remove any game objects
	RemovingObjects();

	///function to spawn new player
	NewPlayer();

	///check if any debris is spawned and dlete if necessary
	if (debrisBits.size() != 0) {
		DeleteParticales(&debrisBits);
	}

	///function to check the state of the game
	CheckLevelWinorLose();
}
void GameState::SetUpLights(){
	/// grab the data for the default shader used for rendering 3D geometry
	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();

	/// set the ambient light
	default_shader_data.set_ambient_light_colour(gef::Colour(0.25f, 0.25f, 0.25f, 1.0f));

	/// add a point light that is almost white, but with a blue tinge
	/// the position of the light is set far away so it acts light a directional light
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(80.f, 80.f, 80.f, 1.f));
	default_point_light.set_position(gef::Vector4(0.0f, 5.0f, 80.0f));
	default_shader_data.AddPointLight(default_point_light);
}
void GameState::GetScreenPosRay(const gef::Vector2& screen_position, const gef::Matrix44& projection, const gef::Matrix44& view, gef::Vector4& startPoint, gef::Vector4& direction, float width, float height) {
	gef::Vector2 ndc;

	float hw = width * 0.5f;
	float hh = height * 0.5f;

	ndc.x = (static_cast<float>(screen_position.x) - hw) / hw;
	ndc.y = (hh - static_cast<float>(screen_position.y)) / hh;

	gef::Matrix44 projectionInverse;
	projectionInverse.Inverse(view * projection);

	gef::Vector4 nearPoint, farPoint;

	nearPoint = gef::Vector4(ndc.x, ndc.y, ndc_z_min_, 1.0f).TransformW(projectionInverse);
	farPoint = gef::Vector4(ndc.x, ndc.y, 1.0f, 1.0f).TransformW(projectionInverse);

	nearPoint /= nearPoint.w();
	farPoint /= farPoint.w();

	startPoint = gef::Vector4(nearPoint.x(), nearPoint.y(), nearPoint.z());
	direction = farPoint - nearPoint;
	direction.Normalise();
}
// modified from https://gamedev.stackexchange.com/questions/96459/fast-ray-sphere-collision-code
bool GameState::RaySphereIntersect(gef::Vector4& startPoint, gef::Vector4& direction, gef::Vector4& sphere_centre, float sphere_radius) {
	gef::Vector4 m = startPoint - sphere_centre;
	float b = m.DotProduct(direction);
	float c = m.LengthSqr() - sphere_radius * sphere_radius;

	// Exit if ray’s origin outside sphere (c > 0) and ray pointing away from sphere (b > 0) 
	if (c > 0.0f && b > 0.0f)
		return false;
	float discr = b * b - c;

	// A negative discriminant corresponds to ray missing sphere 
	if (discr < 0.0f)
		return false;

	// Ray now found to intersect sphere, compute smallest t value of intersection
	float t = -b - sqrtf(discr);

	// If t is negative, ray started inside sphere so clamp t to zero 
	if (t < 0.0f)
		t = 0.0f;

	gef::Vector4 hitpoint = startPoint + direction * t;

	return true;
}
/// function to update the sprites position and rotataion, check if user input is valid and play sounds
bool GameState::Selection(gef::InputManager * inputManager, gef::Renderer3D * r, float width, float height) {

	const gef::TouchInputManager * touch_manager = inputManager->touch_manager();
	/// get the active touches for this panel
	const gef::TouchContainer& panel_touches = touch_manager->touches(0);

	if (panel_touches.size() > 0)
	{
		/// just grabbing the first touch for simplicity here
		/// normally we go through all active touches and check the id
		gef::Touch touch = panel_touches.front();

		/// only process this touch if it is NEW or ACTIVE
		if ((touch.type == gef::TT_NEW)) {
			/// convert the touch position to a ray that starts on the camera near plane
			/// and shoots into the camera view frustum
			gef::Vector2 screen_position = touch.position;

			gef::Vector4 ray_start_position, ray_direction;
			GetScreenPosRay(screen_position, r->projection_matrix(), r->view_matrix(), ray_start_position, ray_direction, width, height);
			if (activePlayer->getBody()) {
				/// Create a sphere around the position of the player body
				/// the radius can be changed for larger objects
				/// radius= 0.5f is a sensible value for a 1x1x1 cube
				gef::Vector4 sphere_centre(activePlayer->getBody()->GetPosition().x, activePlayer->getBody()->GetPosition().y, 0.0f);
				float  sphere_radius = 3.f;
				/// check to see if the ray intersects with the bound sphere that is around the player
				if (RaySphereIntersect(ray_start_position, ray_direction, sphere_centre, sphere_radius)) {
					start.x = touch.position.x;
					start.y = touch.position.y;
					activePlayer->setIsSelected(true); /// if the user has touched the pig set isSelected to true
					audio_manager_->PlaySample(selected_sfx_id); ///play sound effect
					
				}
			}
		}
		else if (touch.type == gef::TT_ACTIVE)
		{
			touch_position_ = touch.position;
			/// set float variable activeMag to the magnitude of the vector being drawn by the users touch
			/// calculate the rotation and angle based in the touch input,
			/// check the input is within an acceptable range that the player cant be launched backwards
			/// update the sprite accordingly
			activeMag = sqrt(pow(touch_position_.x - start.x, 2) + pow(touch_position_.y - start.y, 2));
			float rotation = 0;
			float pi = 3.1415;
			rotation = atan2(touch.position.y - start.y, touch.position.x + (activeMag / 2) - start.x) * 180 / pi;
			rotation = fmod(rotation + 360, 360);
			rotation = rotation * (pi / 180);
			if (rotation > 1.5 && rotation < 4.5) {
				activePlayer->setIsValid(true);
				slingshotBand.set_position(gef::Vector4(touch_position_.x + (activeMag / 2), touch.position.y, 0));
				slingshotBand.set_rotation(rotation);
				slingshotBand.set_width(activeMag);
			}
			else {
				activePlayer->setIsValid(false);
			}
		}
		else if (touch.type == gef::TT_RELEASED) {
			release_position_ = touch.position;
			if (activePlayer->getIsSelected() && activePlayer->getIsValid()) {
				isLaunchable = true;
				///play sound effects when launched
				audio_manager_->PlaySample(launched_sfx_id); 
				audio_manager_->PlaySample(squeal_sfx_id);
				
				return true;
			}
			else {
				activePlayer->setIsSelected(false);
				return false;
			}
		}
	}
	return false;
}
/// function to check activeplayer has a valid input and launch accordingly
void GameState::Launch() {
	if (activePlayer->getIsValid()) {
		magnitude = sqrt(pow(release_position_.x - start.x, 2) + pow(release_position_.y - start.y, 2));
		distance = release_position_ - start;
		activePlayer->setForce(b2Vec2(distance.x * -8, distance.y * 8));
		activePlayer->getBody()->ApplyForceToCenter(activePlayer->getForce(), true);
		activePlayer->setIsLaunched(true);
		activePlayer->state_ = Player::LAUNCHED;
		isLaunchable = false;
	}
}
void GameState::HandleInput(gef::InputManager* input_manager_, float frame_time)
{
	const gef::SonyController* controller = input_manager_->controller_input()->GetController(0);
	if (input_manager_) {
		input_manager_->Update();

		gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
		if (controller_input) {

			const gef::SonyController* controller = controller_input->GetController(0);

			if (controller) {
				
				activePlayer->HandleInput(controller, input_manager_,frame_time, audio_manager_, &platform_, volInfo);
				
				///if select button is pushed, return game to SplashScreen
				if (controller->buttons_down() & gef_SONY_CTRL_SELECT){
					SceneApp::SetState(new SplashScreen(platform_));
				}
			}
		}
	}
}
void GameState::DrawHUD(gef::SpriteRenderer * spriteRenderer)
{
	if (font_)
	{
		/// display frame rate
		font_->RenderText(spriteRenderer, gef::Vector4(820.0f, 20.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "fps: %f", fps_);
	}
}
void GameState::CollisionDetection() {
	/// collision detection
	/// get the head of the contact list
	b2Contact* contact = world_->GetContactList();
	/// get contact count
	int contact_count = world_->GetContactCount();

	for (int contact_num = 0; contact_num < contact_count; ++contact_num)
	{
		if (contact->IsTouching())
		{
			/// get the colliding bodies
			b2Body* bodyA = contact->GetFixtureA()->GetBody();
			b2Body* bodyB = contact->GetFixtureB()->GetBody();

			GameObject* ptrObjA = (GameObject*)bodyA->GetUserData();
			GameObject* ptrObjB = (GameObject*)bodyB->GetUserData();

			if (ptrObjA != nullptr )
			{
				ptrObjA->CollisionResponse(bodyB, ptrObjB);
			}

			if (ptrObjB != nullptr)
			{
				ptrObjB->CollisionResponse(bodyA, ptrObjA);
			}
		}
	}
	/// Get next contact point
	contact = contact->GetNext();
}
///update the and apply physics to the bodies used within b2world
void GameState::UpdatePhysicsBodies() {

	obstacleManager.UpdatePhysicsBodies();

	enemyManager.UpdatePhysicsBodies();

	activePlayer->UpdateFromSimulation();

	if (debrisBits.size() > 0) {
		for (GameObject * debris : debrisBits) {
			debris->UpdateFromSimulation();
		}
	}
}
void GameState::Pan(gef::InputManager* input_manager_)
{
	if (!activePlayer->getIsSelected()) {
		const gef::TouchInputManager* touch_input = input_manager_->touch_manager();
		if (touch_input && (touch_input->max_num_panels() > 0))
		{
			// get the active touches for this panel
			const gef::TouchContainer& panel_touches = touch_input->touches(0);

			// go through the touches
			for (gef::ConstTouchIterator touch = panel_touches.begin(); touch != panel_touches.end(); ++touch)
			{
				// if active touch id is -1, then we are not currently processing a touch
				if (active_touch_id_ == -1)
				{
					// check for the start of a new touch
					if (touch->type == gef::TT_NEW)
					{
						active_touch_id_ = touch->id;
						start = touch->position;
					}
				}
				else if (active_touch_id_ == touch->id)
				{
					// we are processing touch data with a matching id to the one we are looking for
					if (touch->type == gef::TT_ACTIVE)
					{
						current = touch->position;
						pan += (current.x - start.x) * 0.1;
						start = current;
						if (pan > 200)
							pan = 200;
						if (pan < -200)
							pan = -200;
						camera_.pan_ = pan;
					}
					else if (touch->type == gef::TT_RELEASED)
					{
						// the touch we are tracking has been released
						// perform any actions that need to happen when a touch is released here
						// we're not doing anything here apart from resetting the active touch id
						active_touch_id_ = -1;
					}
				}
			}
		}
	}
}

///function to create debris, loops 6 times to create 6 debris particles
/// give the debris dimensions, setup the mesh for the debris, create GameObject, add GameObject to vector
/// set position to the obstacle that has been collided with, initalize body and update the physics
/// apply force to the body, play breaking sound effect
void GameState::SpawnDebris(Obstacle * obstacle){
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			/// debris dimensions
			gef::Vector4 mesh_half_dimensions(0.2f, 0.1f, 0.2f);
			b2Vec2 body_half_dimensions(0.2f, 0.1f);
			/// setup the mesh for the debris
			const gef::Mesh *m = woodChipAsset.mesh;
			///create GameObject
			GameObject *d = new GameObject();
			d->type_ = GameObject::PARTICLE;
			///add GameObject to vector
			debrisBits.push_back(d);
			///set position to the obstacle that has been collided with, initalize bod and update the physics
			gef::Vector4 pos = gef::Vector4(obstacle->getBody()->GetPosition().x, obstacle->getBody()->GetPosition().y, 0);
			d->initBody(m, b2_dynamicBody, b2Shape::e_polygon, b2Vec2(pos.x() + i * 0.25f, pos.y() + j * 0.25f), world_, body_half_dimensions, .5, .5, GameObject::SOMETIMESCOLLIDE, GameObject::SOMETIMESCOLLIDE);
			d->UpdateFromSimulation();
			/// apply force to the body 
			d->getBody()->ApplyLinearImpulse(b2Vec2(0.01 * randRange(1.f, 1.5f), 0.01 * randRange(-.4f, 0.f)), b2Vec2(randRange(0.085f, 0.115f), randRange(0.085f, 0.115f)), true);
			///play breaking sound effect
			audio_manager_->PlaySample(break_sfx_id);
			
			
		}
	}
}

///function for spawning new player
void GameState::NewPlayer(){
	/// if the active player has been launched and is not alive
	/// check if the player count is less than the MaxPlayers value
	/// delete the active player, pop it from the vector of playrss and add a new instance of player
	/// initilaize the player values
	/// set the active player to the last elemnent of the vector
	///increment the playr count
	/// adjust the gui accordingly
	/// check if the user has lost
	if(activePlayer->getLaunched()){
		if (!activePlayer->getAlive()) {
			if (playerCount < maxPlayers) {
				activePlayer->CleanUp(world_);
				activePlayer->set_mesh(NULL);
				players.pop_back();
				players.push_back(new Player);
				players.back()->type_ = GameObject::PLAYER;
				players.back()->initBody(PlayerAsset.GetMeshFromSceneAssets(PlayerAsset.LoadSceneAssets(platform_, "Meshes/NewLeggyPig.scn")), b2_dynamicBody, b2Shape::e_circle, b2Vec2(-25, 10), world_, b2Vec2(.5, .5), .5, 1, GameObject::ALWAYSCOLLIDE, GameObject::ALWAYSCOLLIDE);
				players.back()->setSpawnPosition(gef::Vector4(players.back()->getBody()->GetPosition().x, players.back()->getBody()->GetPosition().y, 0));
				activePlayer = players.back();
				playerCount++;

				if (gui.sprites.size() >= 1) {
					delete gui.sprites.back();
					gui.sprites.pop_back();
				}
				else {
					levelLost = true;
				}
			}
		}
	}
}
/// fucntion to increase score
void GameState::AddScore()
{
	/// lerp the score from its current score to new score
	score = Lerp(score, score + newScore, 1);
	/// play sound effect
	audio_manager_->PlaySample(score_sfx_id);
	/// set the static score variable to score  
	SceneApp::SetScore(score);
}
