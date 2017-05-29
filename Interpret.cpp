#include "Interpreter.h"
#include <iostream>
#include <string>
#include <fstream>

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

string interpreter(string SQLSentence)
{
	string firstWord;
	int SQLCurrentPointer = 0, end = 0;
	if (SQLSentence.empty())//the sentence is empty
		SQLSentence = getInput();
	while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
		SQLCurrentPointer++;
	end = SQLSentence.find(' ', SQLCurrentPointer);
	firstWord = SQLSentence.substr(SQLCurrentPointer, end);//get the first word from SQL sentence
	SQLCurrentPointer = end + 1;//pointer move forward
	
	///////start to analysis the sentence
	if (firstWord == "select")
	{

	}
	else if (firstWord == "insert")
	{

	}
	else if (firstWord == "update")
	{

	}
	else if (firstWord == "delect")
	{

	}
	else if (firstWord == "exec")
	{

	}
	else if (firstWord == "drop")
	{

	}
	else
	{

	}


	return SQLSentence;
}
int main()
{
	string SQLSentence;
	SQLSentence = interpreter(SQLSentence);
	return 0;
}
