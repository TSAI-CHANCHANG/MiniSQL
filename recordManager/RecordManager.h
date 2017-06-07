//
// Created by Zhao Xiaodong on 2017/6/4.
//

#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <string>
#include <vector>
#include <map>
#include "../table.h"

using namespace std;

enum DataType {
    int_t = 1,
    float_t,
    char_t
};

enum RelationOp {
    ne,
    nlt,
    ngt,
    lt,
    gt,
    eq
};

map<RelationOp, string> relationOps{
        {ne,  "<>"},
        {nlt, ">="},
        {ngt, "<="},
        {lt,  "<"},
        {gt,  ">"},
        {eq,  "="}
};

struct intRestrict {
    intRestrict(string attrName) : attrName(attrName) {}

    string attrName;
    int minValue;
    bool includeMin;
    int maxValue;
    bool includeMax;
    vector<int> excludeValues;
};

struct floatRestrict {
    floatRestrict(string attrName) : attrName(attrName) {}

    string attrName;
    float minValue;
    bool includeMin;
    float maxValue;
    bool includeMax;
    vector<float> excludeValues;
};

struct stringRestrict {
    stringRestrict(string attrName) : attrName(attrName) {}

    string attrName;
    string value;
    vector<string> excludeValues;
};

class RecordManager {
private:
    string tableName;
    Table tableInfo;

    string generateInsertValues(string rawValues, Table tableInfo);

public:
    RecordManager(const string tableName) : tableName(tableName) {}

    vector<intRestrict> parseWhere(string rawWhereClause);
    bool insertRecord(string rawValues);
    bool selectRecords(vector<string> attributes, string rawWhereClause);
    bool deleteRecords(string rawWhereClause);
};


#endif //MINISQL_RECORDMANAGER_H
