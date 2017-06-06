#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

#define ERROR       -1
#define QUIT_NUMBER -2

#define SELECT       1
#define CREATE       2
#define DROP         3
#define INSERT       4
#define QUIT         5
#define EXEC         6
#define DROP_TABLE   7
#define DROP_INDEX   8
#define CREATE_TABLE 9
#define CREATE_INDEX 10
#define DELETE       11
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
	inline void setWhereClause(const string &whereC) { whereClause = whereC; }
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
};
