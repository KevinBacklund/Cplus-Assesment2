#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include "console_renderer.h"


struct Position
{
	int x;
	int y;
	bool operator==(const Position& other) const
	{
		return x == other.x && y == other.y;
	}
};

struct GameState
{
	char map[screen_width][screen_height];
	std::vector<Position> snake;
	Position foodPosition;
	bool gameOver = false;
	int score = 0;
};

struct InputState
{
	bool up = true;
	bool down = false;
	bool left = false;
	bool right = false;
};


void SpawnFood(GameState& gameState)
{
	Position newFoodPosition;
	newFoodPosition.x = rand() % (screen_width - 2);
	newFoodPosition.y = rand() % (screen_height - 2);
	gameState.foodPosition = newFoodPosition;
}

void GrowSnake(GameState& gameState)
{
	Position newSegment = gameState.snake.back();
	gameState.snake.push_back(newSegment);
}

void MoveSnake(GameState& gameState)
{
	Position previousPosition = gameState.snake[0];
	for (int i = 1; i < gameState.snake.size(); i++)
	{
		Position currentPosition = gameState.snake[i];
		gameState.snake[i] = previousPosition;
		previousPosition = currentPosition;
	}
}

void CheckCollision(GameState& gameState)
{
	Position head = gameState.snake[0];
	if (gameState.map[head.x][head.y] == '#')
	{
		std::cout << "Game Over! You hit a wall." << std::endl;
		gameState.gameOver = true;
	}
	for (int i = 1; i < gameState.snake.size(); i++)
	{
		if (head == gameState.snake[i])
		{
			std::cout << "Game Over! You hit yourself." << std::endl;
			gameState.gameOver = true;
		}
	}
	if (head == gameState.foodPosition)
	{
		gameState.score += 1;
		GrowSnake(gameState);
		SpawnFood(gameState);
	}
}

void Input(InputState& input)
{
	if (getIfBasicKeyIsCurrentlyDown('W'))
	{
		input.up = true;
		input.down = false;
		input.left = false;
		input.right = false;
	}
	else if (getIfBasicKeyIsCurrentlyDown('S'))
	{
		input.up = false;
		input.down = true;
		input.left = false;
		input.right = false;
	}
	else if (getIfBasicKeyIsCurrentlyDown('A'))
	{
		input.up = false;
		input.down = false;
		input.left = true;
		input.right = false;
	}
	else if (getIfBasicKeyIsCurrentlyDown('D'))
	{
		input.up = false;
		input.down = false;
		input.left = false;
		input.right = true;
	}
}

void Logic(InputState input, GameState& gamestate)
{
	MoveSnake(gamestate);
	if (input.up)
	{
		gamestate.snake[0].y -= 1;
	}
	else if (input.down)
	{
		gamestate.snake[0].y += 1;
	}
	else if (input.left)
	{
		gamestate.snake[0].x -= 1;
	}
	else if (input.right)
	{
		gamestate.snake[0].x += 1;
	}
	CheckCollision(gamestate);
}

void Render(const GameState& gamestate)
{
	for (int x = 0; x < screen_width; x++)
	{
		for (int y = 0; y < screen_height; y++)
		{
				drawTile(x, y, gamestate.map[x][y]);
		}
	}
	drawTile(gamestate.foodPosition.x, gamestate.foodPosition.y, '*');
	drawTile(gamestate.snake[0].x, gamestate.snake[0].y, '@');
	for (int i = 1; i < gamestate.snake.size(); i++)
	{
		drawTile(gamestate.snake[i].x, gamestate.snake[i].y, 'o');
	}
	renderBuffer();
}

void MakeMap(GameState& gamestate)
{
	for (int x = 0; x < screen_width; x++)
	{
		for (int y = 0; y < screen_height; y++)
		{
			if (x == 0 || x == screen_width - 1 || y == 0 || y == screen_height - 1)
			{
				gamestate.map[x][y] = '#';
			}
			else
			{
				gamestate.map[x][y] = ' ';
			}
		}
	}
}


void GameLoop(InputState& input, GameState& gameState)
{
	while (!gameState.gameOver)
	{
		Input(input);
		Logic(input, gameState);
		Render(gameState);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

int main(int n, char* args[])
{
	int level = 1;
	setupCustomConsole();
	if (n > 1)
	{
		std::string arg = args[1];
		if (arg == "level1")
		{
			level = 1;
		}
		else if (arg == "level2")
		{
			level = 2;
		}
		else if (arg == "level3")
		{
			level = 3;
		}
		else
		{
			std::cout << "Invalid level argument, defaulting to level 1" << std::endl;
			level = 1;
			std::cout << "Usage: " << args[0] << " [level1|level2|level3]" << std::endl;
			std::cout << "Example: " << args[0] << " level2" << std::endl;
		}
	}
	std::cout << "Selected level: " << level << std::endl;
	

	InputState input;
	GameState gameState;
	MakeMap(gameState);
	SpawnFood(gameState);
	gameState.snake.push_back({ screen_width / 2, screen_height / 2 });
	GameLoop(input, gameState);
}
