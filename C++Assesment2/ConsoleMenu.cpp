#include "ConsoleMenu.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

std::map<std::string, std::vector<std::string>> menuOptions = 
{
	{ "Main", { "Level select", "Options"} },
	{ "Options", { "Difficulty", "Controls"} },
	{ "Level select", { "Level 1", "Level 2", "Level 3"} },
	{ "Difficulty", { "Easy", "Medium", "Hard"} },
	{ "Controls", { "WASD", "ArrowKeys"}}
};

int main()
{
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