#include "Interpreter.h"
#include <iostream>
#include <string>
#include <fstream>

#define ERROR       -1
#define QUIT_NUMBER -2

using namespace std;
/////////////////////////////////////////////////
/////Function No. 1:
/////get the user input 
string getInput()
{
	string SQLSentence;
	string temp;
	char str[100];
	bool finish = false;
	while (!finish)
	{
		cin >> str;
		temp = str;
		SQLSentence = SQLSentence + " " + temp;
		if (SQLSentence[SQLSentence.size() - 1] == ';')
		{
			SQLSentence[SQLSentence.size() - 1] = ' ';
			SQLSentence += ";";
			finish = true;
		}
	}
	return SQLSentence;
}

int interpreter(string &SQLSentence, int &fileReadFlag)
{
	string firstWord;
	int SQLCurrentPointer = 0, end = 0;
	static char address[1000];
	ifstream ifs("D:\input.txt");

	//////////////////////input part/////////////////////////////////////////////
	if (SQLSentence.empty() && fileReadFlag == 1)//the sentence is empty and now it is time to read file
	{
		getline(ifs, SQLSentence);
		cout << SQLSentence << endl;
	}
	else if(SQLSentence.empty())
		SQLSentence = getInput();
	//////////////////////instruction analysis part//////////////////////////////////


	while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
		SQLCurrentPointer++;
	end = SQLSentence.find(' ', SQLCurrentPointer);
	firstWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);//get the first word from SQL sentence
	SQLCurrentPointer = end;//pointer move forward
	errno_t  err;
	///////start to analysis the sentence
	if (firstWord == "select")
	{
		while (SQLSentence[SQLCurrentPointer] != ';')//get rid of the ';' from the beginning of the sentence
			SQLCurrentPointer++;
	}
	else if (firstWord == "insert")
	{

	}
	else if (firstWord == "quit")
	{
		string nextWord;
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
			SQLCurrentPointer++;
		end = SQLSentence.find(' ', SQLCurrentPointer);
		nextWord = SQLSentence.substr(SQLCurrentPointer, end - 1);
		if (nextWord == ";")			
			return QUIT_NUMBER;
		else
			cout << "Error! Quit instruction should has only one key word \"quit\"\n";
	}
	else if (firstWord == "delect")
	{

	}
	else if (firstWord == "exec")
	{
		string temp;
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
			SQLCurrentPointer++;
		end = SQLSentence.find(' ', SQLCurrentPointer);
		temp = SQLSentence.substr(SQLCurrentPointer + 1, end - 2);
		strcpy(address, temp.c_str());
		if (err = freopen(address, "r", stdin) != 0)
			cout << "The file "<< address <<" was not opened\n";    
		else
		{
			cout << "The file " << address << " was opened\n";
			fileReadFlag = 1;
		}
		SQLCurrentPointer = end;
	}
	else if (firstWord == "drop")
	{

	}
	else
	{
		cout << "Error! Doesn't found a instruction key word!";
	}

	while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
		SQLCurrentPointer++;
	while (SQLSentence[SQLCurrentPointer] == ';')//get rid of the ' ' from the beginning of the sentence
		SQLCurrentPointer++;
	SQLSentence.erase(0, SQLCurrentPointer);
	return 0;
}
int main(int argc, char *argv[]) 
{
	string SQLSentence;
	int conditionCode = 0;
	int stop = 0;
	int fileReadFlag = 0;
	while (!stop)
	{
		conditionCode = interpreter(SQLSentence, fileReadFlag);
		if (conditionCode == QUIT_NUMBER)
			stop = 1;
	}
	return 0;
}
