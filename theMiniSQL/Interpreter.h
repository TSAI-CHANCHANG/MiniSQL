#pragma once
#include "Interpreter.h"
#include "buffermanager.h"
#include "catalogmanager.h"
#include "RecordManager.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

#define ERROR       -10001
#define QUIT_NUMBER -10002

#define SELECT       10001
#define CREATE       10002
#define DROP         10003
#define INSERT       10004
#define QUIT         10005
#define EXEC         10006
#define DROP_TABLE   10007
#define DROP_INDEX   10008
#define CREATE_TABLE 10009
#define CREATE_INDEX 10010
#define DELETE       10011
#define EXEC         10012
string getInput();

class condition
{
	int instructionType;
	vector <string> colName;
	string tableName;
	string indexName;
	string whereClause;
	string insertValues;
	string attribute;
public:
	condition() : instructionType(NULL) {}
	~condition() {}
	inline void setInstruction(int type) { instructionType = type; }//set the instruction type
	inline void setColName(const string &col) { colName.push_back(col); }
	inline void setTableName(const string &table) { tableName = table; }
	inline void setIndexName(const string &index) { indexName = index; }
	inline void setWhereClause(const string &whereC) { whereClause =whereC; }
	inline void setInsertValues(const string &insertV) { insertValues = insertV; }
	inline void setAttribute(const string &attributeS) { attribute = attributeS; }
	inline void clearClass() 
	{ 
		instructionType = 0; 
		colName.clear(); 
		tableName = ""; 
		indexName = ""; 
		whereClause = "";
		attribute = "";
		insertValues = "";
	}
	inline int showInstruction() const { return instructionType; }
	inline vector <string> showColName() const{ return colName; }
	inline string showTableName() const { return tableName; }
	inline string showIndexName() const { return indexName; }
	inline string showWhereClause() const { return whereClause; }
	inline string showInsertValues() const { return insertValues; }
	inline string showAttribute() const { return attribute; }
};

string getInput();
int select_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition);
int drop_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition);
int insert_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition);
int create_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition);
int delect_clauese(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition);
int interpreter(string &SQLSentence, condition &SQLCondition, BPLUSTREE &BTree, buffermanager &bufManager);
void API(condition &SQLCondition, BPLUSTREE &BTree, buffermanager &bufManager);