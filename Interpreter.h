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
string getInput();

class condition
{
	int instructionType;
	vector <string> colName;
	string tableName;
	string indexName;
	vector <string> whereClause;
	string insertValues;
public:
	condition() : instructionType(NULL) {}
	~condition() {}
	inline void setInstruction(int type) { instructionType = type; }//set the instruction type
	inline void setColName(const string &col) { colName.push_back(col); }
	inline void setTableName(const string &table) { tableName = table; }
	inline void setIndexName(const string &index) { indexName = index; }
	inline void setWhereClause(const string &whereC) { whereClause.push_back(whereC); }
	inline void clearClass() { instructionType = 0; colName.clear(); tableName = ""; indexName = ""; whereClause.clear(); }
	inline void setInsertValues(const string &insertV) { insertValues = insertV; }
};
