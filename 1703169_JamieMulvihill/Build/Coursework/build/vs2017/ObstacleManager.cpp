#include "ObstacleManager.h"
#include <system/file.h>

ObstacleManager::~ObstacleManager()
{
}

/// function to populate the list of obstacles, reads the postions from a file and loops for every position in the vector
/// creates a list iterator, adds an obstacles to list, sets iterator to be the first value of list, advances iterator to correct position in list according to i value,
/// initializes body values, checks if the i values is % 0, if it is set obstacle to horizontal and sets obstacles iterator to current iterator
void ObstacleManager::InitObstacles(const gef::Mesh * meshH, const gef::Mesh * meshV, b2BodyType btype_, b2Shape::Type bshape_, b2World * world_, b2Vec2 halfsizeH, b2Vec2 halfsizeV, float radius, float density, uint16 categoryBits, uint16 maskBits, const char * filename)
{
	LoadObstaclesFromFile(filename);

	for (int i = 0; i < obstaclePositions.size(); i++) {
			std::list<Obstacle*>::iterator it;
			obstacles.push_back(new Obstacle);
			it = obstacles.begin();
			advance(it, i);

			if (i % 3 == 0) {
				obstacles.back()->state_ = Obstacle::HORIZONTAL;
				obstacles.back()->initBody(meshH, btype_, bshape_, obstaclePositions[i], world_, halfsizeH, radius, density, categoryBits, maskBits);
			}
			else {
				obstacles.back()->state_ = Obstacle::VERTICAL;
				obstacles.back()->initBody(meshV, btype_, bshape_, obstaclePositions[i], world_, halfsizeV, radius, density, categoryBits, maskBits);
			}

			obstacles.back()->setAlive(true);
			obstacles.back()->type_ = GameObject::OBSTACLE;
			obstacles.back()->SetIterator(it);
	}

}

/// function to iteratre through the list and render all the obstacles that are alive
void ObstacleManager::Render(gef::Renderer3D * renderer)
{
	for (std::list<Obstacle*>::iterator it = obstacles.begin(); it != obstacles.end(); ++it) {
		if ((*it)->getAlive())
		{
			Obstacle *current = *it;
			renderer->DrawMesh(*current);
		}
	}
}

/// function to delete obstacles, calls the gameObject clean up function and erases obstacle from list by getting its iterator
void ObstacleManager::DeleteObstacle(b2World * world_, Obstacle * obstacle)
{
	obstacle->CleanUp(world_);
	obstacles.erase(obstacle->GetIterator());
}

///function to update the bodies of alive obstacles
void ObstacleManager::UpdatePhysicsBodies()
{
	for (std::list<Obstacle*>::iterator it = obstacles.begin(); it != obstacles.end(); ++it) {
		if ((*it)->getAlive())
		{
			Obstacle *current = *it;
			current->UpdateFromSimulation();
		}
	}
}

///read position values from file and store in a vector to be used for obstacles
char* ObstacleManager::FileReader(const char* filename) {

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
void ObstacleManager::LoadObstaclesFromFile(const char* filename)
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
		obstaclePositions.push_back(b2Vec2(positionsValues[i], positionsValues[i + 1]));
	}

	delete input;
	input = NULL;
}
