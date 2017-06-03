#include "Interpreter.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

static char address[1000];
static ifstream ifs;
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

/////////////////////////////////////////////////
/////Function No. 2:
/////analusis the select clause then change the object of condition class 
int select_clause(string &SQLSentence,  int &SQLCurrentPointer, int &end, condition &SQLCondition)
{
	string currentWord;
	//step1
	SQLCondition.setInstruction(SELECT);

	//step2
	if ((end = SQLSentence.find(" from", SQLCurrentPointer)) == -1)//find " from"
	{
		cout << "Error! Can not find key words \"from\"." << endl;//can not find from
		return ERROR;
	}

	//step3
	while (SQLSentence[end] == ' ')//select xxx from ooo, this is to find where is the end of xxx
		--end;
	while (SQLSentence[SQLCurrentPointer] == ' ' && end >= SQLCurrentPointer)//select xxx from ooo, this is to find where is the front of xxx
		++SQLCurrentPointer;
	end++;//adjust the pointer to the space right after xxx
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);

	//step4
	if (currentWord.empty() == 1)//can not find col name
	{
		cout << "Error! Can not find col name." << endl;
		return ERROR;
	}

	//step5
	if (currentWord != "*")//not select all col name
	{
		int f = 0, e = 0;
		string col;
		while (e < currentWord.size())
		{
			if (currentWord[e] == ' ')
			{
				int flag = 0;
				if (currentWord[e - 1] != ',')
					flag = 1;
				while (currentWord[e] == ' ' && e < currentWord.size())
					++e;
				if (flag && currentWord[e] != ',')
				{
					cout << "Error! col name must be separated by \',\'.";
					return ERROR;
				}
				else if (currentWord[e] == ',')
					f = ++e;
				else
					f = e;
			}
			while (currentWord[e] != ',' && currentWord[e] != ' ' && e < currentWord.size())
				++e;
			col = currentWord.substr(f, e - f);
			SQLCondition.setColName(col);
			f = e + 1;//move to next col
			if (currentWord[e] == ',')
				++e;
			if (currentWord[e] == ' ')
			{
				int flag = 0;
				if (currentWord[e - 1] != ',')
					flag = 1;
				while (currentWord[e] == ' ' && e <= currentWord.size())
					++e;
				if (e + 1 == currentWord.size() && flag == 1)
				{
					cout << "Error! col name has syntax error." << endl;
					return ERROR;
				}
				if (flag && currentWord[e] != ',')
				{
					cout << "Error! col name must be separated by \',\'.";
					return ERROR;
				}
				else if (currentWord[e] == ',')
					f = ++e;
				else
					f = e;
			}
		}

	}
	else
	{
		SQLCondition.setColName(currentWord);
	}

	//step6
	while (SQLSentence[end] == ' ')//select xxx from ooo, this is to find where is the front of from
		++end;
	SQLCurrentPointer = end;

	//step7
	end = SQLSentence.find(' ', SQLCurrentPointer);
	if (end == -1)
	{
		cout << "Error! Can not find table name." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}
	SQLCurrentPointer = end;

	//step8
	while (SQLSentence[end] == ' ')//select xxx from ooo, this is to find where is the front of ooo
		++end;
	SQLCurrentPointer = end;
	while (SQLSentence[end] != ';' && SQLSentence[end] != ' ')
		++end;
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);

	//step9
	if (currentWord.empty() || currentWord == "where")
	{
		cout << "Error! the table name can not be found" << endl;
		return ERROR;
	}

	//step10
	SQLCondition.setTableName(currentWord);
	//step11
	while (SQLSentence[end] == ' ')
		++end;
	SQLCurrentPointer = end;
	if (SQLSentence[end] == ';')
		return SELECT;
	else
	{
		///////to be continued
		return SELECT;
	}
}

/////////////////////////////////////////////////
/////Function No. 3:
/////analusis the drop clause then change the object of condition class 
int drop_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition)
{
	string currentWord;
	//step1
	SQLCondition.setInstruction(DROP);

	//step2
	end = SQLSentence.find("table", SQLCurrentPointer);
	if (end != -1)//create table
	{
		SQLCurrentPointer = end;//move pointer to 't'("drop table oooo")
		while (SQLSentence[end] != ' ' && end < SQLSentence.size())//move across "table"
			++end;
		while (SQLSentence[end] == ' ' && end < SQLSentence.size())
			++end;
		if (SQLSentence[end] == ';')//lose table name
		{
			cout << "Error! can not find table name." << endl;
			return ERROR;
		}

	}
	else//create index
	{

	}
}
int interpreter(string &SQLSentence, int &fileReadFlag, condition &SQLCondition)
{
	string firstWord;
	int SQLCurrentPointer = 0, end = 0;
	int code = 0;


	//////////////////////input part/////////////////////////////////////////////
	if (SQLSentence.empty() && fileReadFlag == 1)//the sentence is empty and now it is time to read file
	{
		getline(ifs, SQLSentence);
		if (SQLSentence.empty())//the file has come to end, but doesn't have a quit command;
		{
			cout << "ERROR! This file doesn't have a quit command\n" << endl;
			fileReadFlag = 0;//back to keyboard input
			SQLSentence = "";
			ifs.close();
			return QUIT_NUMBER;
		}
		cout << SQLSentence << endl;
	}
	else if(SQLSentence.empty())
		SQLSentence = getInput();
	//////////////////////instruction analysis part//////////////////////////////////


	while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
		SQLCurrentPointer++;
	end = SQLSentence.find(' ', SQLCurrentPointer);
	if (end == -1)
	{
		end = SQLSentence.find(';', SQLCurrentPointer);//example:"quit;", then there does not have a ' '.
	}
	firstWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);//get the first word from SQL sentence
	SQLCurrentPointer = end;//pointer move forward



	///////start to analysis the sentence
	if (firstWord == "select")
	{
		code = select_clause(SQLSentence, SQLCurrentPointer, end, SQLCondition);
	}
	else if (firstWord == "insert")
	{

	}
	else if (firstWord == "quit")
	{
		string nextWord;
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' after the first word
			SQLCurrentPointer++;
		end = SQLSentence.find(' ', SQLCurrentPointer);//get the next word
		nextWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);//check whether the quit instruction
																				  //has other words. example:"quit select;"
		if (nextWord == ";")//we can only find ';'	
			return QUIT_NUMBER;
		else
			cout << "Error! Quit instruction should has only one key word \"quit\"\n";
	}
	else if (firstWord == "delect")
	{

	}
	else if (firstWord == "exec")//execute the script
	{
		string temp;
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
			SQLCurrentPointer++;
		end = SQLSentence.find(' ', SQLCurrentPointer);
		temp = SQLSentence.substr(SQLCurrentPointer + 1, end - SQLCurrentPointer - 2);
		strcpy(address, temp.c_str());//copy the address to array
		if (freopen(address, "r", stdin) == 0)//can not open
		{
			cout << "The file " << address << " was not opened\n";
		}  
		else
		{
			cout << "The file " << address << " was opened\n";
			fileReadFlag = 1;
			ifs.open(address);
		}
		SQLCurrentPointer = end;
	}
	else if (firstWord == "drop")
	{
		code = drop_clause(SQLSentence, SQLCurrentPointer, end, SQLCondition);
	}
	else//input is wrong
	{
		cout << "Error! Doesn't found a instruction key word!";
		return ERROR;
	}
	if (SQLSentence[SQLCurrentPointer + 1] == ';')
		SQLCurrentPointer += 2;
	else
	{
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' in the rest fo the sentence
			SQLCurrentPointer++;
		while (SQLSentence[SQLCurrentPointer] == ';')//find ';'
			SQLCurrentPointer++;

	}
	
	SQLSentence.erase(0, SQLCurrentPointer);//clear
	return code;
}

int main(int argc, char *argv[]) // this is just a test main function
{
	string SQLSentence;
	condition SQLCondition;
	int conditionCode = 0;
	int stop = 0;
	int fileReadFlag = 0;
	while (!stop)
	{
		conditionCode = interpreter(SQLSentence, fileReadFlag, SQLCondition);
		if (conditionCode == QUIT_NUMBER)
			stop = 1;
	}
	cout << "Press Any Key to Continue..." << endl;
	getchar();
	return 0;
}
