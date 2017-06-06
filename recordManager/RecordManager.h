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
    string generateInsertValues(string rawValues, Table tableInfo);
public:
    RecordManager() {}
    bool insertRecord(string tableName, string rawValues);
//    bool deleteRecords(string tableName, string rawWhereClause);
//    bool selectRecords(string tableName, vector<string> attributes, string rawWhereClause);
};


#endif //MINISQL_RECORDMANAGER_H
