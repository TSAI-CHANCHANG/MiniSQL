//
// Created by Zhao Xiaodong on 2017/6/4.
//

#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <string>
#include <vector>
#include "../table.h"

using namespace std;

enum DataType {
    int_t = 1,
    float_t,
    char_t
};

class RecordManager {
private:
    string tableName;
    Table tableInfo;

    string generateInsertValues(string rawValues, Table tableInfo);

public:
    RecordManager(const string tableName) : tableName(tableName) {}

    bool insertRecord(string rawValues);
    bool selectRecords(vector<string> attributes, string rawWhereClause);
//    bool deleteRecords(string tableName, string rawWhereClause);
};


#endif //MINISQL_RECORDMANAGER_H
