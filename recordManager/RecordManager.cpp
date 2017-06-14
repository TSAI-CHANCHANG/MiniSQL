//
// Created by Zhao Xiaodong on 2017/6/4.
//

#include <iostream>
#include <string>
#include <strstream>
#include <algorithm>
#include <regex>
#include "RecordManager.h"
#include "../index+catalog+buffer/catalogmanager.h"
#include "../index+catalog+buffer/buffermanager.h"
#include "../index+catalog+buffer/table.h"

#define DEBUG_IT true

using namespace std;

bool RecordManager::insertRecord(string rawValues) {
    catalogmanager catalogMgr;
    tableInfo = catalogMgr.GetTable(tableName);

    const bool NO_SUCH_TABLE_ERROR = false; // for test
    if (NO_SUCH_TABLE_ERROR) {
        // TODO: 没有该表如何
        cerr << "Error: " << "No such table named: " << tableName << endl;
    }

    vector<RawData> indexInfos;

    string resultRecord = generateInsertValues(rawValues, indexInfos); // rawData used for get index info
#if DEBUG_IT
    cout << "Insert record:" << endl;
    cout << resultRecord;
    cout << "TableName: " << tableName << endl;
#endif

    buffermanager bufferMgr;

    int blockNum;
    int blockOffset;

    // TODO: resultRecord的最终形式的确定，换行？空格？
    bufferMgr.FindSuitBlockinBuffer(tableName + ".rec", resultRecord.length(), &blockNum, &blockOffset);
#if DEBUG_IT
    cout << "blockNum: " << blockNum << " offset: " << blockOffset << endl;
#endif
    if (resultRecord != "") {
        bufferMgr.Insert(blockNum, blockOffset, (char *)resultRecord.c_str()); // TODO: 返回值？成功失败的标志？
    }

#if DEBUG_IT
#endif

    if (resultRecord != "") {
        // TODO: update index, after specify index Manager interface
//    BPLUSTREE bplustree(blockNum); // ??? blockNum是啥？
    }

    return true;
}

string RecordManager::generateInsertValues(string rawValues, vector<RawData> &indexInfos) {
    string resultRecord;
    int tmp_i;
    float tmp_f;
    string tmp_s;

    // TODO: not check space in string
    rawValues.erase(remove_if(rawValues.begin(), rawValues.end(), [](char x) { return isspace(x); }), rawValues.end());
    istrstream strIn(rawValues.c_str(), rawValues.length());

    for (auto iter = tableInfo.Attr.begin(); iter != tableInfo.Attr.end(); iter++) {
        if (iter != tableInfo.Attr.begin() && iter != tableInfo.Attr.end())
            resultRecord += " ";

        string valueTerm;

        getline(strIn, valueTerm, ','); // TODO: not check comma in string
        istrstream strTermIn(valueTerm.c_str(), valueTerm.length());
        switch ((*iter).type) {
            case int_t:
                strTermIn >> tmp_i; // TODO: check typeError
                resultRecord += to_string(tmp_i);
                break;
            case float_t:
                strTermIn >> tmp_f;
                resultRecord += to_string(tmp_f);
                break;
            case char_t:
                strTermIn >> tmp_s;
                if (*tmp_s.begin() != '\'' || *(tmp_s.end() - 1) != '\'') {
                    cerr << "Error: " << "string input syntax error!\n" << valueTerm << endl;
                    return "";
                }
                tmp_s.erase(tmp_s.begin());
                tmp_s.erase(tmp_s.end() - 1);
                resultRecord += tmp_s;
                break;
            default:
                break;
        }
        if (strTermIn.bad()) {
            cerr << "Error: " << "value not match error!" << endl;
            return "";
        }
        if ((strTermIn >> tmp_s)) {
            cerr << "Error: " << "value type syntax error!" << endl;
#if DEBUG_IT
            cout << tmp_s << endl;
#endif
            return "";
        }

        if ((*iter).unique || (*iter).primary) {
//            checkUnique() TODO: check unique: after done select
        }

        if ((*iter).indexname != "noindex") {
            // TODO: indexname 如何使用
            RawData rawData;
            rawData.type = static_cast<DataType>((*iter).type);
            rawData.value = valueTerm;
            indexInfos.push_back(rawData);
        }
    }
    if (strIn.bad()) {
        cerr << "Error: " << "values list error!" << endl;
        return "";
    }
    if ((strIn >> tmp_s)) {
        cerr << "Error: " << "values list not match error!" << endl;
#if DEBUG_IT
        cout << tmp_s << endl;
#endif
        return "";
    }
    resultRecord += "\n";

    return resultRecord;
}

bool RecordManager::selectRecords(vector<string> attributes, string rawWhereClause) {

    return true;
}

bool RecordManager::deleteRecords(string rawWhereClause = "") {
    if (rawWhereClause.empty()) {
        // TODO: simple fetch all
    } else {
        vector<Restrict *> restricts = parseWhere(rawWhereClause); // TODO: 记得delete掉

    }
    return true;
}

vector<Restrict *> RecordManager::parseWhere(string rawWhereClause) {
    vector<Restrict *> restricts;
    regex andRegex("\\s+and\\s+");
    sregex_token_iterator
            first{rawWhereClause.begin(), rawWhereClause.end(), andRegex, -1},
            last;
    vector<string> restrictions{first, last};

    for (string restriction : restrictions) {
        RelationOp relationOp = invalidOp;
        for (int i = (int)ne; i != (int)eq + 1; i++) {
            RelationOp op = static_cast<RelationOp>(i);
            if (restriction.find(relationOps.find(op)->second) != string::npos) {
                relationOp = op;
            }
        }
        if (relationOp == invalidOp) {
            cerr << "Error: " << "yet unsupported operation." << endl;
            return vector<Restrict *>(0);
        }
        string attrName = restriction.substr(0, restriction.find(relationOps.find(relationOp)->second));
        attrName.erase(remove_if(attrName.begin(), attrName.end(), [](char x) { return isspace(x); }), attrName.end());

        string value = restriction.substr(restriction.find(relationOps.find(relationOp)->second), string::npos);
        value.erase(value.begin(), value.begin() + (relationOps.find(relationOp)->second).length());
        value.erase(remove_if(value.begin(), value.end(), [](char x) { return isspace(x); }), value.end());

#if DEBUG_IT
        cout << "[" << attrName << "]" << relationOps.find(relationOp)->second << "[" << value << "]" << endl;
#endif

#if 1
        vector<Attribute>::iterator attrIter;
        for (attrIter = tableInfo.Attr.begin(); attrIter != tableInfo.Attr.end(); attrIter++) {
            if (attrIter->attrname == attrName) {
                istrstream valueStrIn(value.c_str(), value.length());

                string tmp;
                switch (attrIter->type) {
                    case int_t: {
                        int tmp_i;
                        if (valueStrIn >> tmp_i && !(valueStrIn >> tmp)) {
                            Restrict *intRestrict = new IntRestrict(attrName, relationOp, tmp_i);
                            restricts.push_back(intRestrict);
                        } else {
                            cerr << "Error: " << " type not match!" << endl;
                        }
                        break;
                    }
                    case float_t: {
                        float tmp_f;
                        if (valueStrIn >> tmp_f && !(valueStrIn >> tmp)) {
                            Restrict *floatRestrict = new FloatRestrict(attrName, relationOp, tmp_f);
                            restricts.push_back(floatRestrict);
                        } else {
                            cerr << "Error: " << " type not match!" << endl;
                        }
                        break;

                    }
                    case char_t: {
                        string tmp_s;
                        if (valueStrIn >> tmp_s) {
                            if (*tmp_s.begin() == '\'' && *(tmp_s.end() - 1) == '\'') {
                                Restrict *stringRestrict =
                                        new StringRestrict(attrName, relationOp, tmp_s.substr(1, tmp_s.length() - 2));
                                restricts.push_back(stringRestrict);
                            }
                        } else {
                            cerr << "Error: " << " type not match!" << endl;
                        }
                        break;
                    }
                    default:
                        break;
                }
            } else {
                cerr << "Error: " << "No such attribute" << endl;
                return vector<Restrict *>(0);
            }
        }
#endif
    }

#if DEBUG_IT
    for (Restrict *restrict : restricts) {
        cout << restrict->attrName << relationOps.find(restrict->op)->second << restrict->op;
        switch (restrict->type) {
            case int_t:
                cout << ((IntRestrict *)(restrict))->value << endl;
            case float_t:
                cout << ((FloatRestrict *)(restrict))->value << endl;
            case char_t:
                cout << ((StringRestrict *)(restrict))->value << endl;
        }
    }
#endif
    return restricts;
}

vector<Range *> RecordManager::generateRange(vector<Restrict *> restricts) {
    vector<Range *> ranges;
    for (Restrict *restrict : restricts) {
        string attrName = restrict->attrName;
        vector<Range *>::iterator rangeIter = find_if(ranges.begin(), ranges.end(), [attrName](Range *range) -> bool {
            return range->attrName == attrName;
        });

        if (rangeIter == ranges.end()) {
            switch (restrict->type) {
                case int_t: {
                    IntRange *intRange = new IntRange(attrName);
                    IntRestrict *intRestrict = static_cast<IntRestrict *>(restrict);
                    switch (intRestrict->op) {
                        case ne:
                            intRange->excludeValues.push_back(intRestrict->value);
                            break;
                        case nlt:
                            intRange->minValue = intRestrict->value;
                            intRange->includeMin = true;
                            break;
                        case ngt:
                            intRange->maxValue = intRestrict->value;
                            intRange->includeMax = true;
                            break;
                        case lt:
                            intRange->maxValue = intRestrict->value;
                            intRange->includeMax = false;
                            break;
                        case gt:
                            intRange->minValue = intRestrict->value;
                            intRange->includeMin = false;
                            break;
                        case eq:
                            intRange->minValue = intRange->maxValue = intRestrict->value;
                            intRange->includeMax = intRange->includeMin = true;
                            break;
                        default:
                            break;
                    }
                    ranges.push_back(static_cast<Range *>(intRange));
                    break;
                }
                case float_t: {
                    FloatRange *floatRange = new FloatRange(attrName);
                    FloatRestrict *floatRestrict = static_cast<FloatRestrict *>(restrict);
                    switch (floatRestrict->op) {
                        case ne:
                            floatRange->excludeValues.push_back(floatRestrict->value);
                            break;
                        case nlt:
                            floatRange->minValue = floatRestrict->value;
                            floatRange->includeMin = true;
                            break;
                        case ngt:
                            floatRange->maxValue = floatRestrict->value;
                            floatRange->includeMax = true;
                            break;
                        case lt:
                            floatRange->maxValue = floatRestrict->value;
                            floatRange->includeMax = false;
                            break;
                        case gt:
                            floatRange->minValue = floatRestrict->value;
                            floatRange->includeMin = false;
                            break;
                        case eq:
                            floatRange->minValue = floatRange->maxValue = floatRestrict->value;
                            floatRange->includeMax = floatRange->includeMin = true;
                            break;
                        default:
                            break;
                    }
                    ranges.push_back(static_cast<Range *>(floatRange));
                    break;
                }
                case char_t: {
                    StringRange *stringRange = new StringRange(attrName);
                    StringRestrict *stringRestrict = static_cast<StringRestrict *>(restrict);
                    switch (stringRestrict->op) {
                        case ne:
                            stringRange->excludeValues.push_back(stringRestrict->value);
                            break;
                        case eq:
                            stringRange->value = stringRestrict->value;
                        default:
                            break;
                    }
                    break;
                }
            }
        } else {
            if (!updateRange(*rangeIter, restrict)) {
                ranges.empty();
                return ranges;
            }
        }
    }
    return ranges;
}

bool RecordManager::updateRange(Range *range, Restrict *restrict) {
    switch (restrict->type) {
        case int_t: {
            IntRestrict *intRestrict = static_cast<IntRestrict *>(restrict);
            IntRange *intRange = static_cast<IntRange *>(range);
            switch (restrict->op) {
                case ne: {
                    intRange->excludeValues.push_back(intRestrict->value);
                    break;
                }
                case nlt: {
                    if (intRestrict->value >= intRange->maxValue && !intRange->includeMax ||
                        intRestrict->value > intRange->maxValue) {
                        return false;
                    }
                    if (intRestrict->value > intRange->minValue) {
                        intRange->minValue = intRestrict->value;
                        intRange->includeMin = true;
                    }
                    break;
                }
                case ngt: {
                    if (intRestrict->value <= intRange->minValue && !intRange->includeMin ||
                        intRestrict->value < intRange->minValue) {
                        return false;
                    }
                    if (intRestrict->value < intRange->maxValue) {
                        intRange->maxValue = intRestrict->value;
                        intRange->includeMax = true;
                    }
                    break;
                }
                case lt: {
                    if (intRestrict->value < intRange->minValue ||
                        intRestrict->value == intRange->minValue && !intRange->includeMin) {
                        return false;
                    }
                    if (intRestrict->value < intRange->maxValue ||
                        intRestrict->value == intRange->maxValue && intRange->includeMax) {
                        intRange->maxValue = intRestrict->value;
                        intRange->includeMax = false;
                    }
                    break;
                }
                case gt: {
                    if (intRestrict->value > intRange->maxValue ||
                        intRestrict->value == intRange->maxValue && !intRange->includeMax) {
                        return false;
                    }
                    if (intRestrict->value > intRange->minValue ||
                        intRestrict->value == intRange->minValue && intRange->includeMin) {
                        intRange->minValue = intRestrict->value;
                        intRange->includeMin = false;
                    }
                    break;
                }
                case eq:
                    if (intRestrict->value > intRange->maxValue ||
                        intRestrict->value < intRange->minValue ||
                        intRestrict->value == intRange->maxValue && !intRange->includeMax ||
                        intRestrict->value == intRange->minValue && !intRange->includeMin) {
                        return false;
                    }
                    if (intRange->excludeValues.end() !=
                        find(intRange->excludeValues.begin(), intRange->excludeValues.end(), intRestrict->value)) {
                        return false;
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        case float_t: { // TODO: need to refactor
            FloatRestrict *floatRestrict = static_cast<FloatRestrict *>(restrict);
            FloatRange *floatRange = static_cast<FloatRange *>(range);
            switch (restrict->op) {
                case ne: {
                    floatRange->excludeValues.push_back(floatRestrict->value);
                    break;
                }
                case nlt: {
                    if (floatRestrict->value >= floatRange->maxValue && !floatRange->includeMax ||
                        floatRestrict->value > floatRange->maxValue) {
                        return false;
                    }
                    if (floatRestrict->value > floatRange->minValue) {
                        floatRange->minValue = floatRestrict->value;
                        floatRange->includeMin = true;
                    }
                    break;
                }
                case ngt: {
                    if (floatRestrict->value <= floatRange->minValue && !floatRange->includeMin ||
                        floatRestrict->value < floatRange->minValue) {
                        return false;
                    }
                    if (floatRestrict->value < floatRange->maxValue) {
                        floatRange->maxValue = floatRestrict->value;
                        floatRange->includeMax = true;
                    }
                    break;
                }
                case lt: {
                    if (floatRestrict->value < floatRange->minValue ||
                        floatRestrict->value == floatRange->minValue && !floatRange->includeMin) {
                        return false;
                    }
                    if (floatRestrict->value < floatRange->maxValue ||
                        floatRestrict->value == floatRange->maxValue && floatRange->includeMax) {
                        floatRange->maxValue = floatRestrict->value;
                        floatRange->includeMax = false;
                    }
                    break;
                }
                case gt: {
                    if (floatRestrict->value > floatRange->maxValue ||
                        floatRestrict->value == floatRange->maxValue && !floatRange->includeMax) {
                        return false;
                    }
                    if (floatRestrict->value > floatRange->minValue ||
                        floatRestrict->value == floatRange->minValue && floatRange->includeMin) {
                        floatRange->minValue = floatRestrict->value;
                        floatRange->includeMin = false;
                    }
                    break;
                }
                case eq:
                    if (floatRestrict->value > floatRange->maxValue ||
                        floatRestrict->value < floatRange->minValue ||
                        floatRestrict->value == floatRange->maxValue && !floatRange->includeMax ||
                        floatRestrict->value == floatRange->minValue && !floatRange->includeMin) {
                        return false;
                    }
                    if (floatRange->excludeValues.end() !=
                        find(floatRange->excludeValues.begin(), floatRange->excludeValues.end(),
                             floatRestrict->value)) {
                        return false;
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        case char_t: {
            StringRestrict *stringRestrict = static_cast<StringRestrict *>(restrict);
            StringRange *stringRange = static_cast<StringRange *>(range);
            switch (restrict->op) {
                case ne: {
                    if (stringRange->value == stringRestrict->value) {
                        return false;
                    }
                    if (stringRange->value == "") {
                        stringRange->excludeValues.push_back(stringRestrict->value);
                    }
                    break;
                }
                case eq: {
                    if (stringRange->value != "" && stringRange->value != stringRestrict->value) {
                        return false;
                    }
                    if (stringRange->excludeValues.end() !=
                        find(stringRange->excludeValues.begin(), stringRange->excludeValues.end(),
                             stringRestrict->value)) {
                        return false;
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return true;
}

