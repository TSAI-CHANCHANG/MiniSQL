#include "Interpreter.h"
#include "buffermanager.h"
#include "catalogmanager.h"
#include "RecordManager.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;


int main(int argc, char *argv[]) // this is just a test main function
{
	buffermanager bufManager;
	BPLUSTREE BTree(BLOCKSIZE);
	string SQLSentence;
	condition SQLCondition;
	int conditionCode = 0;
	int stop = 0;
	int fileReadFlag = 0;
	while (!stop)
	{
		conditionCode = interpreter(SQLSentence, SQLCondition, BTree, bufManager);
		if (conditionCode == QUIT_NUMBER)
		{
			stop = 1;
			break;
		}
		if (conditionCode == ERROR)
		{
			SQLCondition.clearClass();
			continue;
		}
		
		
		SQLCondition.clearClass();
	}
	cout << "Press Any Key to Continue..." << endl;
	return 0;
}
