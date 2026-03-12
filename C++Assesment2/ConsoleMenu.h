#pragma once
#include <vector>
#include <string>
#include <windows.h>

struct Settings
{
	unsigned short snakeColour = FOREGROUND_GREEN;
	int tickRate = 10;
};

void Menu(Settings& gameSettings);
void ProcessCommands(std::vector<std::string>& path, Settings& gameSettings);
std::string RecieveCommand(std::vector<std::string>& path);
bool IsValidCommand(std::vector<std::string>& path, std::string& command);
std::string Capitalize(std::string& str);
std::vector<std::vector<std::string>> LoadHighscore();
void SaveHighscore(int score, int level, std::string name);
void ShowHighscores(std::vector<std::vector<std::string>>, int level);
std::vector<std::vector<char>> LoadMap(int level);
int CheckAvailableLevels();

void StartGame(int level, Settings gameSettings);
std::vector<std::vector<char>> MakeMap(int mapWidth, int mapHeight, int level);

