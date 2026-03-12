#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <sstream>
#include <algorithm>
#include "console_renderer.h"
#include "ConsoleMenu.h"

struct Position
{
	int x;
	int y;

	bool operator==(const Position& other) const
	{
		return x == other.x && y == other.y;
	}
	Position& operator+=(const Position& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}
	Position operator+(const Position& other) const
	{
		return { x + other.x, y + other.y };
	}
};

struct GameState
{
	std::vector<std::vector<char>> map;
	std::vector<Position> snake;
	Position foodPosition;
	Position moveDirection{ 0, -1 };
	bool gameOver = false;
	int score = 0;
	int level = 1;
};

struct InputState
{
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	bool operator==(const InputState& other) const
	{
		return up == other.up && down == other.down && left == other.left && right == other.right;
	}
};


void GameOver(const GameState* gameState);

std::vector<std::vector<char>> MakeMap(int mapWidth, int mapHeight, int level)
{
	int obstacleCount = 0;
	std::vector<std::vector<char>> map;
	map.resize(mapWidth, std::vector<char>(mapHeight, ' '));

	if (level > 1)
	{
		obstacleCount = level * 5;
	}

	std::vector<Position> obstacles;
	for (int i = 0; i < obstacleCount; i++)
	{
		Position obstacle;
		obstacle.x = rand() % (mapWidth - 2);
		obstacle.y = rand() % (mapHeight - 2);
		obstacles.push_back(obstacle);
	}

	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
			if (x == 0 || x == mapWidth - 1 || y == 0 || y == mapHeight - 1)
			{
				map[x][y] = '#';
			}
			else if (std::find(obstacles.begin(), obstacles.end(), Position{ x, y }) != obstacles.end())
			{
				map[x][y] = '#';
			}
			else
			{
				map[x][y] = ' ';
			}
		}
	}
	return map;
}

void SpawnFood(GameState* gameState)
{
	int mapHeight = gameState->map.size();
	int mapWidth = gameState->map[0].size();

	Position newFoodPosition;
	newFoodPosition.x = rand() % (mapWidth - 2);
	newFoodPosition.y = rand() % (mapHeight - 2);
	while (gameState->map[newFoodPosition.y][newFoodPosition.x] == '#' || std::find(gameState->snake.begin(), gameState->snake.end(), newFoodPosition) != gameState->snake.end())
	{
		newFoodPosition.x = rand() % (mapWidth - 2);
		newFoodPosition.y = rand() % (mapHeight - 2);
	}
	gameState->foodPosition = newFoodPosition;
}

void GrowSnake(GameState* gameState)
{
	Position newSegment = gameState->snake.back();
	gameState->snake.push_back(newSegment);
}

void MoveTail(GameState& gameState)
{
	Position previousPosition = gameState.snake[0];
	for (int i = 1; i < gameState.snake.size(); i++)
	{
		Position currentPosition = gameState.snake[i];
		gameState.snake[i] = previousPosition;
		previousPosition = currentPosition;
	}
}

void CheckCollision(GameState* gameState)
{
	Position head = gameState->snake[0];
	if (gameState->map[head.y][head.x] == '#')
	{
		gameState->gameOver = true;
	}
	for (int i = 1; i < gameState->snake.size(); i++)
	{
		if (head == gameState->snake[i])
		{
			gameState->gameOver = true;
		}
	}
	if (head == gameState->foodPosition)
	{
		gameState->score += 1;
		GrowSnake(gameState);
		SpawnFood(gameState);
	}
}

void Input(InputState& input)
{
	InputState newInput;

	if (getIfBasicKeyIsCurrentlyDown('W'))
	{
		newInput.up = true;
	}
	if (getIfBasicKeyIsCurrentlyDown('S'))
	{
		newInput.down = true;
	}
	if (getIfBasicKeyIsCurrentlyDown('A'))
	{
		newInput.left = true;
	}
	if (getIfBasicKeyIsCurrentlyDown('D'))
	{
		newInput.right = true;
	}

	InputState noInput;
	if (newInput != noInput) 
	{
		input = newInput;
	}
}

void Logic(InputState input, GameState* gameState)
{
	MoveTail(*gameState);

	if (input.up || input.down || input.left || input.right)
	{
		Position previousDirection = gameState->moveDirection;
		Position zero{ 0, 0 };
		gameState->moveDirection = { 0, 0 };
		if (input.up)
		{
			gameState->moveDirection.y =  -1;
		}
		else if (input.down)
		{
			gameState->moveDirection.y = 1;
		}
		if (input.left)
		{
			gameState->moveDirection.x = -1;
		}
		else if (input.right)
		{
			gameState->moveDirection.x = 1;
		}
		if (gameState->moveDirection == zero || gameState->moveDirection + previousDirection == zero)
		{
			gameState->moveDirection = previousDirection;
		}
	}
	gameState->snake[0] += gameState->moveDirection;

	CheckCollision(gameState);
}

void Render(const GameState* gameState)
{
	int mapHeight = gameState->map.size();
	int mapWidth = gameState->map[0].size();
	std::string score = "Snake Game - Score: " + std::to_string(gameState->score);

	drawString(mapWidth + 3, 3, score);

	for (int x = 0; x < mapWidth; x++)
	{
		for (int y = 0; y < mapHeight; y++)
		{
				drawTile(x, y, gameState->map[y][x]);
		}
	}
	drawTile(gameState->foodPosition.x, gameState->foodPosition.y, '*', FOREGROUND_RED);
	drawTile(gameState->snake[0].x, gameState->snake[0].y, '@', FOREGROUND_GREEN);
	for (int i = 1; i < gameState->snake.size(); i++)
	{
		drawTile(gameState->snake[i].x, gameState->snake[i].y, 'o', FOREGROUND_GREEN);
	}
	renderBuffer();
}

void GameLoop(InputState& input, GameState* gameState)
{
	int logicTPS = 7;
	int logicTickDuration = 1000000 / logicTPS;
	std::chrono::duration logicTickDurationChrono = std::chrono::microseconds(logicTickDuration);
	std::chrono::microseconds msSinceLogicTick (0);
	std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
	while (!gameState->gameOver)
	{
		std::chrono::duration timeSinceLastFrame = std::chrono::steady_clock::now() - lastTime;
		msSinceLogicTick += std::chrono::duration_cast<std::chrono::microseconds>(timeSinceLastFrame);
		lastTime = std::chrono::steady_clock::now();

		Input(input);
		if (msSinceLogicTick >= logicTickDurationChrono)
		{
			msSinceLogicTick -= logicTickDurationChrono;
			Logic(input, gameState);
		}

		Render(gameState);
	}
}

void StartGame(int level)
{
	setupCustomConsole();
	InputState input;
	GameState *gameState = new GameState;
	gameState->level = level;
	input.up = true;
	gameState->map = LoadMap(gameState->level);
	int mapHeight = gameState->map.size();
	int mapWidth = gameState->map[0].size();
	SpawnFood(gameState);
	gameState->snake.push_back({ mapWidth / 2, mapHeight / 2 });
	GameLoop(input, gameState);
	GameOver(gameState);
}

void GameOver(const GameState* gameState)
{
	clearBuffer();
	deleteCustomConsole();
	std::cout << "Game Over! Your score: " << gameState->score << std::endl;
	std::vector<std::vector<std::string>> highscores = LoadHighscore();
	if (highscores.size() < gameState->level)
	{
		highscores.resize(gameState->level);
	}
	std::vector<std::string>& levelHighscores = highscores[gameState->level - 1];
	ShowHighscores(highscores, gameState->level);
	if (levelHighscores.size() < 10 || gameState->score > std::stoi(levelHighscores.back().substr(levelHighscores.back().find(":") + 1)))
	{
		std::cout << "Congratulations! You made it to the highscore list!" << std::endl;
		std::cout << "Please enter your name: ";
		std::string name;
		std::getline(std::cin, name);
		SaveHighscore(gameState->score, gameState->level, name);
	}
	delete gameState;
	Menu();
}

