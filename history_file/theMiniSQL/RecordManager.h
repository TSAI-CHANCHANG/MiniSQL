//
// Created by Zhao Xiaodong on 2017/6/4.
//

#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <limits>
#include "table.h"
#include "buffermanager.h"
#include "catalogmanager.h"
#include "IndexManager.h"

using namespace std;

enum DataType {
    int_t = 1,
    myFloat,
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

struct IndexInfo {
    string indexName;
    DataType type;
    string value;
};

struct KeysWithType {
    DataType type;
    vector<string> keys;
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
            Restrict(attrName, op, myFloat), value(value) {}

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
            Range(true, myFloat, attrName),
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
    catalogmanager &catalogMgr;
    buffermanager &bufferMgr;
    BPLUSTREE &bPlusTree;

    string generateInsertValues(string rawValues, vector<IndexInfo> &indexInfos);
    string projectTuple(string tuple, vector<string> &values);

    void checkSelectTuple(vector<Range *> &ranges, vector<string> &attributes);
    void checkSelectTupleInBuffer(vector<Range *> &ranges, vector<string> &attributes);

    void checkDeleteTuple(vector<Range *> &ranges);
    void checkDeleteTupleInBuffer(vector<Range *> &ranges);

    bool inIntRange(IntRange *range, int val);
    bool inFloatRange(FloatRange *range, float val);
    bool inStringRange(StringRange *range, string val);

    bool checkInRange(vector<Range *> &ranges, map<string, string> &valueOfAttr);
    bool updateRange(Range *range, Restrict *restrict); // TODO: be private

    bool checkUnique(Attribute attr, string value);

    vector<Restrict *> parseWhere(string rawWhereClause = nullptr);
    vector<Range *> generateRange(const vector<Restrict *> &restricts);

public:
    RecordManager(const string tableName, catalogmanager &catalogMgr, buffermanager &bufferMgr, BPLUSTREE &bPlusTree)
            : tableName(tableName), catalogMgr(catalogMgr), bufferMgr(bufferMgr), bPlusTree(bPlusTree) {}

    bool insertRecord(string rawValues);
    bool selectRecords(vector<string> &attributes, string rawWhereClause);
    bool deleteRecords(string rawWhereClause);
};


#endif //MINISQL_RECORDMANAGER_H
