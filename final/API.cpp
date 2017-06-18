#include "Interpreter.h"
#include "buffermanager.h"
#include "catalogmanager.h"
#include "RecordManager.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void API(condition &SQLCondition, BPLUSTREE &BTree)
{
	buffermanager bufManager;
	catalogmanager catalogManager(bufManager);
	RecordManager recordManager(SQLCondition.showTableName(), catalogManager, bufManager, BTree);
	int instruction;
	instruction = SQLCondition.showInstruction();
	switch (instruction)
	{
	case INSERT:
		recordManager.insertRecord(SQLCondition.showInsertValues());
		break;
	case SELECT:
		recordManager.selectRecords(SQLCondition.showColName(), SQLCondition.showWhereClause());
		break;
	case DELETE:
		recordManager.deleteRecords(SQLCondition.showWhereClause());
		break;
	case CREATE_TABLE:
		catalogManager.CreateTable(SQLCondition.showTableName(), SQLCondition.showAttribute(), BTree);
		break;
	case CREATE_INDEX:
		catalogManager.CreateIndex(SQLCondition.showIndexName(), SQLCondition.showTableName(), SQLCondition.showAttribute(), BTree);
		break;
	case DROP_TABLE:
		catalogManager.DropTable(SQLCondition.showTableName(), BTree);
		break;
	case DROP_INDEX:
		catalogManager.DropIndex(SQLCondition.showIndexName(), SQLCondition.showTableName(), BTree);
		break;
	default:
		break;
	}
}
extern block blocks[BLOCKNUMBER];
int main(int argc, char *argv[]) // this is just a test main function
{
	BPLUSTREE BTree(BLOCKSIZE);
	string SQLSentence;
	condition SQLCondition;
	int conditionCode = 0;
	int stop = 0;
	int fileReadFlag = 0;
	while (!stop)
	{
		conditionCode = interpreter(SQLSentence, fileReadFlag, SQLCondition);
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
		API(SQLCondition, BTree);

		SQLCondition.clearClass();
	}
	cout << "Press Any Key to Continue..." << endl;
	return 0;
}