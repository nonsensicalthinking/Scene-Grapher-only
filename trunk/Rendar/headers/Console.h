/*
 * Console.h
 *
 *  Created on: Jun 13, 2010
 *      Author: Derek Brooks
 */
#include "Font.h"
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <stdarg.h>
#include <map>

#ifndef CONSOLE_H_
#define CONSOLE_H_

#define MAX_CONSOLE_LINE_LEN	128
#define MAX_CONSOLE_COMMANDS	1024

using namespace std;

typedef struct consoleCommand_t	{
	string name;
	void (*func)();
	void (*func1)(string);
	bool hasArgs;
}cmd_t;



class Console {
public:

//	cmd_t registeredCommands[MAX_CONSOLE_COMMANDS];

	map<string, cmd_t*> registeredCommands;

	bool consoleActive;
	string inputString;
	ostringstream* instr;
	list<string> *input;
	int cmdHistoryIndex;
	list<string> *output;
	int minusIndex;
	Font* font;

	int screenWidth;
	int screenHeight;

	Console(int width, int height);
	virtual ~Console();

	void scrollDown();
	void scrollUp();
	void previousCommand();
	void nextCommand();
	void appendToInput(unsigned char s);
	void removeLastCharacter();
	void clearInput();
	void processConsoleCommand(const std::string conInput);
	void Draw();
	void con_print(string s);
	void registerCommand(string name, void (*func)());
	void registerCommand(string name, void (*func)(string), bool hasArgs);

};

#endif /* CONSOLE_H_ */
