//
// Created by Zhao Xiaodong on 2017/6/4.
//

#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <limits>
#include "../index+catalog+buffer/table.h"

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

static map<RelationOp, string> relationOps{
        {ne,  "<>"},
        {nlt, ">="},
        {ngt, "<="},
        {lt,  "<"},
        {gt,  ">"},
        {eq,  "="}
};

struct Restrict {
    Restrict() {}

    Restrict(string attrName, RelationOp op, DataType type) : attrName(attrName), op(op), type(type) {}

    string attrName;
    RelationOp op;
    DataType type;
};

struct IntRestrict : Restrict {
    IntRestrict(string attrName, RelationOp op, int value) :
            Restrict(attrName, op, int_t), value(value) {}

    int value;
};

struct FloatRestrict : Restrict {
    FloatRestrict(string attrName, RelationOp op, float value) :
            Restrict(attrName, op, float_t), value(value) {}

    float value;
};

struct StringRestrict : Restrict {
    StringRestrict(string attrName, RelationOp op, string value) :
            Restrict(attrName, op, char_t), value(value) {}

    string value;
};

#if 1

struct Range {
    Range() {}
    Range(bool valid, DataType type, string attrName) :
            valid(valid), type(type), attrName(attrName) {}

    bool valid;
    DataType type;
    string attrName;
};

struct IntRange : Range {
    IntRange(string attrName) :
            Range(true, int_t, attrName),
            minValue(numeric_limits<int>::min()), maxValue(numeric_limits<int>::max()),
            includeMin(false), includeMax(false) {}

    int minValue;
    bool includeMin;
    int maxValue;
    bool includeMax;
    vector<int> excludeValues;
};

struct FloatRange : Range {
    FloatRange(string attrName) :
            Range(true, float_t, attrName),
            minValue(numeric_limits<float>::min()), maxValue(numeric_limits<float>::max()),
            includeMin(false), includeMax(false) {}

    float minValue;
    bool includeMin;
    float maxValue;
    bool includeMax;
    vector<float> excludeValues;
};

struct StringRange : Range {
    StringRange(string attrName) :
            Range(true, char_t, attrName) {}

    string value;
    vector<string> excludeValues;
};

#endif

class RecordManager {
private:
    string tableName;
    Table tableInfo;

    string generateInsertValues(string rawValues);

public:
    RecordManager(const string tableName) : tableName(tableName) {}

    vector<Restrict *> parseWhere(string rawWhereClause = nullptr); // TODO: be private
    vector<Range *> generateRange(vector<Restrict *> restricts); // TODO: be private
    bool updateRange(Range *range, Restrict *restrict); // TODO: be private
    bool insertRecord(string rawValues);
    bool selectRecords(vector<string> attributes, string rawWhereClause);
    bool deleteRecords(string rawWhereClause);
};


#endif //MINISQL_RECORDMANAGER_H
