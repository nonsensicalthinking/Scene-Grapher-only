/*
 * Console.cpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Derek Brooks
 */
#include "strtools.h"
#include "Console.h"
#include "keys.h"
#include <iostream>
#include <list>
#include "shared.h"
using namespace std;

extern void shutdown();
extern void Con_print(const char* fmt, ...);

Console::Console(int width, int height) {
	consoleActive = false;
	input = new list<string>;
	instr = new ostringstream;
	output = new list<string>;	// console output
	font = new Font(width, height);
	screenWidth = width;
	screenHeight = height;
	minusIndex = 1;
	cmdHistoryIndex = 0;
	output->push_back("");	// FIXME: need 1 bogus entry in the console
}

Console::~Console() {

	map<string, cmd_t*>::iterator itr;
	for(itr=registeredCommands.begin();itr!=registeredCommands.end();itr++)	{
		cmd_t* cmd = (*itr).second;
		delete cmd;
	}

	delete input;
	delete instr;
	delete output;
	delete font;
}

void Console::scrollDown()	{
	if( minusIndex > 1 )
		minusIndex--;
}

void Console::scrollUp()	{
	if( minusIndex < output->size() )
		minusIndex++;
}

void Console::previousCommand()	{

	if( cmdHistoryIndex < 0 )
		cmdHistoryIndex = 0;
	else if( cmdHistoryIndex > input->size()-1 )
		cmdHistoryIndex = input->size()-1;

	cout << "CMDHISTORYINDEX++: " << cmdHistoryIndex << endl;

	list<string>::iterator cmdHistoryItr;

	cmdHistoryItr = input->begin();
	for(int x=0; x < cmdHistoryIndex; x++)	{
		cmdHistoryItr++;
	}

	inputString = (*cmdHistoryItr);
	cmdHistoryIndex++;
}

void Console::nextCommand()	{

	if( cmdHistoryIndex < 0 )
		cmdHistoryIndex = 0;
	else if( cmdHistoryIndex > input->size()-1 )
		cmdHistoryIndex = input->size()-1;

	cout << "CMDHISTORYINDEX--: " << cmdHistoryIndex << endl;

	list<string>::iterator cmdHistoryItr;

	cmdHistoryItr = input->begin();
	for(int x=0; x < cmdHistoryIndex; x++)	{
		cmdHistoryItr++;
	}

	inputString = (*cmdHistoryItr);
	cmdHistoryIndex--;
}

void Console::appendToInput(unsigned char s)	{
	if( s == ENTER_KEY )	{
		(*instr) << s;

		string coninput = instr->str();

		clearInput();

		processConsoleCommand(coninput);
	}
	else if( s == BACKSPACE_KEY )	{
		removeLastCharacter();
		inputString = instr->str();
	}
	else	{	// just append
		(*instr) << s;
		inputString = instr->str();
	}

}

void Console::removeLastCharacter()	{
	string s = instr->str();
	clearInput();
	(*instr) << s.substr(0, s.length()-1);
}

void Console::clearInput()	{
//	delete instr;
	instr = new ostringstream;
	inputString = "";
}


// TODO make these changeable and/or dependent upon environment
#define left 		0
#define lines		20	// number of visible lines on the screen
#define lineHeight	16	// line height in pixels

void Console::Draw()	{
	int lineNumber = lines;
	list<string>::iterator strItr;

	// Console's background
	glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
		glLoadIdentity();
		glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();
			GLfloat emis[] = {0.0, 0.0, 0.0};	// Make it black
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
			glBegin(GL_QUADS);
			glVertex2i(0,screenHeight-((lines+1)*lineHeight));
			glVertex2i(screenWidth, screenHeight-((lines+1)*lineHeight));
			glVertex2i(screenWidth, screenHeight);
			glVertex2i(0,screenHeight);
			glEnd();
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);

    // set font color
    glColor3f(0.0, 1.0, 0.0);

	// Seek scroll position
	strItr = output->end();
	for(int x=0; x < minusIndex; x++)
		strItr--;

	// iterate and draw the console output backward so we can see most recent output
	for(; (strItr!=output->begin() && lineNumber >= 0); strItr--, lineNumber--)
		font->glPrint(left, (screenHeight-(lineNumber*16)), (*strItr).c_str(), 0);

	string s = ">" + inputString;

	// draw input line
	font->glPrint(left, screenHeight-((lines+1)*lineHeight), s.c_str(), 0);

}

void Console::con_print(string s)	{
	output->push_back(s);
}


void Console::processConsoleCommand(const string conInput)	{
	// Don't proceed without real input
	if( conInput[0] == '\n' || conInput[0] == '\r' )
		return;

	string input = conInput.substr(0, conInput.length()-1);	// remove junk character

	// Save command to history.
	// and save to console output
	cmdHistoryIndex = 0;
	this->input->push_front(input);
	this->output->push_back(">" + input);

	// Process command
	string cmd = input.substr(0, input.find_first_of(" "));
	string content = input.substr(input.find_first_of(" ")+1);

	map<string, cmd_t*>::iterator itr = registeredCommands.find(cmd);

	if( itr != registeredCommands.end() )	{
		cmd_t* command = (*itr).second;
		if( !command->hasArgs )	{
			void (*f1)() = command->func;
			(*f1)();
		}
		else	{
			void (*f2)(string) = command->func1;
			(*f2)(content);
		}

	}
	else
		Con_print("Command not found: %s", cmd.c_str());

}


void Console::registerCommand(string name, void (*func)())	{
	cmd_t* cmd = new cmd_t;
	cmd->name = name;
	cmd->func = func;
	cmd->hasArgs = false;

	if( registeredCommands.find(cmd->name) == registeredCommands.end() )	{
		registeredCommands[cmd->name] = cmd;
	}
	else	{
		Con_print("Hash Conflict: %s has identical hash to %s", name.c_str(), name.c_str());
	}
}

void Console::registerCommand(string name, void (*func)(string), bool hasArgs)	{
	cmd_t* cmd = new cmd_t;
	cmd->name = name;
	cmd->func1 = func;
	cmd->hasArgs = true;

	if( registeredCommands.find(cmd->name) == registeredCommands.end() )	{
		registeredCommands[cmd->name] = cmd;
	}
	else	{
		Con_print("Hash Conflict: %s has identical hash to %s", name.c_str(), name.c_str());
	}
}








