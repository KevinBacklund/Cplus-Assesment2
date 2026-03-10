#pragma once
#include <vector>
#include <string>

void Menu();
void NavigateMenu(std::vector<std::string>& path);
std::string RecieveCommand(std::vector<std::string>& path);
bool IsValidCommand(std::vector<std::string>& path, std::string& command);
std::string Capitalize(std::string& str);

void StartGame(int level);