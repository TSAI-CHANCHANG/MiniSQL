//
// Created by Zhao Xiaodong on 2017/6/4.
//

#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <limits>
#include "../table.h"

using namespace std;

enum DataType {
    int_t = 1,
    float_t,
    char_t
};

enum RelationOp {
    invalidOp,
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

struct Restrict {
    Restrict() {}

    Restrict(string attrName, RelationOp op) : attrName(attrName), op(op) {}

    string attrName;
    RelationOp op;
    DataType type;
};

struct IntRestrict : Restrict {
    IntRestrict(string attrName, RelationOp op, int value) : attrName(attrName), op(op), value(value), type(int_t) {}

    int value;
};

struct FloatRestrict : Restrict {
    FloatRestrict(string attrName, RelationOp op, float value) :
            attrName(attrName), op(op), value(value), type(float_t) {}

    float value;
};

struct StringRestrict : Restrict {
    StringRestrict(string attrName, RelationOp op, string value) :
            attrName(attrName), op(op), value(value), type(char_t) {}

    string value;
};

#if 0
struct Restrict {
    Restrict(bool valid = true) : valid(valid) {}

    bool valid;
    DataType type;
    string attrName;
};

struct IntRestrict : Restrict {
    IntRestrict() : type(int_t), minValue(numeric_limits<int>::min()), maxValue(numeric_limits<int>::max()),
                    includeMin(false), includeMax(false) {}

    int minValue;
    bool includeMin;
    int maxValue;
    bool includeMax;
    vector<int> excludeValues;
};

struct FloatRestrict : Restrict {
    FloatRestrict() : type(float_t), minValue(numeric_limits<float>::min()), maxValue(numeric_limits<float>::max()),
                      includeMin(false), includeMax(false) {}

    float minValue;
    bool includeMin;
    float maxValue;
    bool includeMax;
    vector<float> excludeValues;
};

struct StringRestrict : Restrict {
    StringRestrict() {
        type = char_t;
    }

    string attrName;
    string value;
    vector<string> excludeValues;
};

#endif

class RecordManager {
private:
    string tableName;
    Table tableInfo;

    string generateInsertValues(string rawValues, Table tableInfo);

public:
    RecordManager(const string tableName) : tableName(tableName) {}

    vector<Restrict> parseWhere(string rawWhereClause);
    bool insertRecord(string rawValues);
    bool selectRecords(vector<string> attributes, string rawWhereClause);
    bool deleteRecords(string rawWhereClause);
};


#endif //MINISQL_RECORDMANAGER_H
