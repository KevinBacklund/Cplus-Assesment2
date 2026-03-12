#include "ConsoleMenu.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>

std::map<std::string, std::vector<std::string>> menuOptions = 
{
	{ "Main", { "Level select", "Options", "Highscore"}},
	{ "Options", { "Difficulty"} },
	{ "Level select", {}},
};

int main()
{
	int availableLevels = CheckAvailableLevels();
	for (int i = 1; i <= availableLevels; i++)
	{
		menuOptions["Level select"].push_back("Level " + std::to_string(i));
	}
	Menu();
}

void Menu()
{
	std::vector<std::string> path;
	path.push_back("Main");

	NavigateMenu(path);

	std::cout << "Welcome to the Snake Game!" << std::endl;
	std::cout << "Press Enter to start..." << std::endl;
	std::cin.get();
}

void NavigateMenu(std::vector<std::string>& path)
{
	std::string currentMenu = path.back();
	std::string currentPath;
	for (std::string& menu : path)
	{
		currentPath += menu + " > ";
	}
	std::cout << currentPath << std::endl;
	
	std::string command = RecieveCommand(path);

	if (!IsValidCommand(path, command))
	{
		std::cout << "Invalid command, try again." << std::endl;
		NavigateMenu(path);
	}

	if (command == "Quit")
	{
		exit(0);
	}
	else if (command == "Back")
	{
		if (path.size() > 1)
		{
			std::vector<std::string> newPath(path.begin(), path.end() - 1);
			NavigateMenu(newPath);
		}
		else
		{
			std::cout << "Already at main." << std::endl;
			NavigateMenu(path);
		}
	}
	else if (command == "Level 1" || command == "Play")
	{
		StartGame(1);
	}
	else if (command == "Level 2")
	{
		StartGame(2);
	}
	else if (command == "Level 3")
	{
		StartGame(3);
	}
	else if (command == "Highscore")
	{
		std::vector<std::vector<std::string>> highscores = LoadHighscore();
		ShowHighscores(highscores, -1);
		NavigateMenu(path);
	}
	else
	{
		std::vector<std::string> newPath(path);
		newPath.push_back(command);
		NavigateMenu(newPath);
	}
}

std::string RecieveCommand(std::vector<std::string>& path)
{
	std::string currentMenu = path.back();
	std::vector<std::string> options = menuOptions[currentMenu];
	std::cout << "Available Commands:\n";
	for (std::string& option : options)
	{
		std::cout << option << "\n";
	}
	std::cout << "Back\nQuit" << std::endl;

	std::cout << "\nEnter command: ";
	std::string command;
	std::getline(std::cin, command);
	command = Capitalize(command);
	return command;
}

bool IsValidCommand(std::vector<std::string>& path, std::string& command)
{
	std::string currentMenu = path.back();
	std::vector<std::string> options = menuOptions[currentMenu];
	for (std::string& option : options)
	{
		if (option == command)
		{
			return true;
		}
	}
	if (command == "Back" || command == "Quit" || command == "Play")
	{
		return true;
	}
	return false;
}

std::string Capitalize(std::string& str)
{
	std::string result = str;
	result[0] = toupper(result[0]);
	for (int i = 1; i < result.length(); i++)
	{
		result[i] = tolower(result[i]);
	}
	return result;
}

std::vector<std::vector<std::string>> LoadHighscore()
{
	std::vector<std::vector<std::string>> highscores;
	std::ifstream highscoresFile;
	highscoresFile.open("highscores.txt");
	if (highscoresFile.is_open())
	{
		std::string line;

		while (std::getline(highscoresFile, line))
		{
			int level = std::stoi(line.substr(0, line.find(";")));
			if (highscores.size() < level)
			{
				highscores.resize(level);
			}
			highscores[level-1].push_back(line);
		}

		highscoresFile.close();
	}
	return highscores;
}

void SaveHighscore(int score, int level, std::string name)
{
	std::vector<std::vector<std::string>> highscores = LoadHighscore();
	if (highscores.size() < level)
	{
		highscores.resize(level);
	}
	std::vector<std::string>& levelHighscores = highscores[level-1];
	levelHighscores.push_back(std::to_string(level) + "; " + name + ": " + std::to_string(score));
	std::sort(levelHighscores.begin(), levelHighscores.end(), [](const std::string& a, const std::string& b) {
		int scoreA = std::stoi(a.substr(a.find(":") + 1));
		int scoreB = std::stoi(b.substr(b.find(":") + 1));
		return scoreA > scoreB;
	});
	if (levelHighscores.size() > 10)
	{
		levelHighscores.pop_back();
	}
	std::ofstream highscoresFile;
	highscoresFile.open("highscores.txt");
	if (highscoresFile.is_open())
	{
		for (int i = 0; i < highscores.size(); i++)
		{
			for (std::string& line : highscores[i])
			{
				highscoresFile << line << "\n";
			}
		}
		highscoresFile.close();
	}
}

void ShowHighscores(std::vector<std::vector<std::string>> highscores, int level)
{
	std::cout << "Highscores:\n";
	if (level != -1)
	{
		std::cout << "Level " << level << ":\n";
		for (std::string& line : highscores[level-1])
		{
			std::cout << line.substr(line.find(";") + 2) << "\n";
		}
	}
	else
	{
		for (int i = 0; i < highscores.size(); i++)
		{
			std::cout << "Level " << i+1 << ":\n";
			for (std::string& line : highscores[i])
			{
				std::cout << line.substr(line.find(";") + 2) << "\n";
			}
			std::cout << "\n";
		}
	}
	std::cout << std::endl;
}

int CheckAvailableLevels()
{
	int levelCount = 0;
	while (true)
	{
		std::ifstream levelFile;
		levelFile.open("level" + std::to_string(levelCount + 1) + ".txt");
		if (levelFile.is_open())
		{
			levelCount++;
			levelFile.close();
		}
		else if (levelCount < 3)
		{
			for (int i = levelCount; i < 3; i++)
			{
				std::ofstream levelFile;
				levelFile.open("level" + std::to_string(i + 1) + ".txt");
				std::vector<std::vector<char>> map = MakeMap(80, 25, i + 1);
				for (int y = 0; y < map[0].size(); y++)
				{
					for (int x = 0; x < map.size(); x++)
					{
						levelFile << map[x][y];
					}
					levelFile << "\n";
				}
				levelFile.close();
			}
		}
		else
		{
			break;
		}
	}
	return levelCount;
}

std::vector<std::vector<char>> LoadMap(int level)
{
	std::vector<std::vector<char>> map;
	std::ifstream levelFile;
	levelFile.open("level" + std::to_string(level) + ".txt");
	if (levelFile.is_open())
	{
		std::string line;
		while (std::getline(levelFile, line))
		{
			std::vector<char> row(line.begin(), line.end());
			map.push_back(row);
		}
		levelFile.close();
	}
	return map;
}