#include "EnemyManager.h"
#include <system/file.h>


EnemyManager::~EnemyManager()
{
}

/// function to populate the list of enemies, reads the postions from a file and loops for every position in the vector
/// creates a list iterator, adds an enemy to list, sets iterator to be the first value of list, advances iterator to correct position in list according to i value,
/// initializes body values and sets enemy iterator to current iterator
void EnemyManager::InitEnemies(const gef::Mesh *mesh, b2BodyType btype_, b2Shape::Type bshape_, b2World * world_, b2Vec2 halfsize, float radius, float density, uint16 categoryBits, uint16 maskBits, const char* filename)
{
	LoadEnemiesFromFile(filename);

	for(int i = 0; i < enemyPositions.size(); i++){
		std::list<Enemy*>::iterator it;
		enemies.push_back(new Enemy);
		it = enemies.begin();
		advance(it, i);
		enemies.back()->initBody(mesh, btype_, bshape_, enemyPositions[i], world_, halfsize, radius, density, categoryBits, maskBits);
		enemies.back()->setAlive(true);
		enemies.back()->type_ = GameObject::ENEMY;
		enemies.back()->SetIterator(it);
	}
}

/// function to iteratre through the list and render all the enemies that are alive
void EnemyManager::Render(gef::Renderer3D* renderer)
{
	for (std::list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
		if ((*it)->getAlive())
		{
			Enemy *current = *it;
			renderer->DrawMesh(*current);
		}
	}
}

/// function to delete enemys, calls the gameObject clean up function and erases enemy from list by getting its iterator 
void EnemyManager::DeleteEnemy(b2World* world_, Enemy* enemy)
{
	enemy->CleanUp(world_);
	enemies.erase(enemy->GetIterator());
}

///function to update the bodies of alive enemies
void EnemyManager::UpdatePhysicsBodies()
{
	for (std::list<Enemy*>::iterator it = enemies.begin(); it != enemies.end(); ++it) {
		if ((*it)->getAlive())
		{
			Enemy *current = *it;
			current->UpdateFromSimulation();
		}
	}
}

/// function to Read enemy postions from file, takes a file name and concatenates string to a .txt file,
///converts values from file to a char* and returns value called point
char* EnemyManager::FileReader(const char* filename) {

	std::vector<float> values;
	std::string config_filename(filename);
	config_filename += ".txt";
	void* file_data = NULL;
	Int32 file_size = 0;
	gef::File* file = gef::File::Create();

	bool success = true;
	success = file->Open(config_filename.c_str());
	if (success)
	{
		success = file->GetSize(file_size);
		if (success)
		{
			file_data = malloc(file_size);
			success = file_data != NULL;
			if (success)
			{
				Int32 bytes_read;
				success = file->Read(file_data, file_size, bytes_read);
				if (success) {
					success = bytes_read == file_size;
				}
			}
		}

		char* point = (char*)file_data;

		file->Close();
		delete file;
		file = NULL;

		return point;
	}

	return nullptr;
}

///function to populate vector using FileReader function, creates a vector of floats, converts the char* returned by FileReader function to string
/// loops through the string and converts to float, converts floats to B2vec2 and populates vector of b2Vec2
void EnemyManager::LoadEnemiesFromFile(const char* filename)
{
	char* input = FileReader(filename);

	std::vector<float> positionsValues;
	std::string string = input;
	std::string output;

	for (int i = 0; i < string.size(); i++)
	{
		char currentChar = string[i];


		if (currentChar >= 48 && currentChar <= 57)
		{
			output += currentChar;
		}

		if (currentChar == ',')
		{

			char LE = 13;
			char CR = 10;

			output += LE;
			output += CR;

			float number;
			number = atof(output.c_str());
			positionsValues.push_back(number);
			output.clear();
		}

	}

	for (int i = 0; i < positionsValues.size(); i += 2)
	{
		enemyPositions.push_back(b2Vec2(positionsValues[i], positionsValues[i + 1]));
	}

	delete input;
	input = NULL;
}