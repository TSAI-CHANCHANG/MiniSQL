//
// Created by Zhao Xiaodong on 2017/6/4.
//

#include <iostream>
#include <string>
#include <strstream>
#include <algorithm>
#include <regex>
#include "RecordManager.h"
#include "../catalogmanager.h"
#include "../buffermanager.h"
#include "../table.h"

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

    string resultRecord = generateInsertValues(rawValues, tableInfo);

#if DEBUG_IT
    cout << "Insert record:" << endl;
    cout << resultRecord;
#endif

    buffermanager bufferMgr;

    int blockNum;
    int blockOffset;

    // TODO: resultRecord的最终形式的确定，换行？空格？
    bufferMgr.FindSuitBlockinBuffer(tableName, resultRecord.length(), &blockNum, &blockOffset);
    bufferMgr.Insert(blockNum, blockOffset, (char *)resultRecord.c_str()); // TODO: 返回值？成功失败的标志？

    // TODO: update index, after specify index Manager interface

    return true;
}

string RecordManager::generateInsertValues(string rawValues, Table tableInfo) {
    string resultRecord;
    int tmp_i;
    float tmp_f;
    string tmp_s;

    // TODO: not check space in string
    rawValues.erase(remove_if(rawValues.begin(), rawValues.end(), [](char x) { return isspace(x); }), rawValues.end());
    istrstream strIn(rawValues.c_str(), rawValues.length());

    for (auto iter = tableInfo.Attr.begin(); iter != tableInfo.Attr.end(); iter++) {
        if (iter != tableInfo.Attr.begin() && iter != tableInfo.Attr.end() - 1)
            resultRecord += " ";

        string valueTerm;

        getline(strIn, valueTerm, ','); // TODO: not check comma in string
        istrstream strTermIn(rawValues.c_str(), rawValues.length());
        switch ((*iter).type) {
            case int_t:
                strTermIn >> tmp_i; // TODO: check typeError
                resultRecord += to_string(tmp_i);
                break;
            case float_t:
                strTermIn >> tmp_f;
                break;
            case char_t:
                if (*valueTerm.begin() != '\'' || *(valueTerm.end() - 1) != '\'') {
                    cerr << "Error: " << "string input syntax error!\n" << valueTerm << endl;
                    return "";
                }
                valueTerm.erase(valueTerm.begin());
                valueTerm.erase(valueTerm.end());
                strTermIn >> valueTerm;
                break;
            default:
                break;
        }
        if (strTermIn.bad()) {
            cerr << "Error: " << "value not match error!" << endl;
            return "";
        }
        if (!(strTermIn >> tmp_s)) {
            cerr << "Error: " << "value type syntax error!" << endl;
            return "";
        }

        if ((*iter).unique || (*iter).primary) {
//            checkUnique() TODO: after done select
        }
    }
    if (strIn.bad()) {
        cerr << "Error: " << "values list error!" << endl;
        return "";
    }
    if (!(strIn >> tmp_s)) {
        cerr << "Error: " << "values list not match error!" << endl;
        return "";
    }
    resultRecord += "\n";

    return resultRecord;
}

bool RecordManager::selectRecords(vector<string> attributes, string rawWhereClause) {

    return true;
}

bool RecordManager::deleteRecords(string rawWhereClause) {

    return true;
}

vector<Restrict> RecordManager::parseWhere(string rawWhereClause) {
    vector<Restrict> restricts;
    regex andRegex("\\s+and\\s+");
    sregex_token_iterator
            first{rawWhereClause.begin(), rawWhereClause.end(), andRegex, -1},
            last;
    vector<string> restrictions{first, last};

    for (string restriction : restrictions) {
        string::iterator iter;
        RelationOp relationOp = invalidOp;
        for (int i = ne; i != eq; i++) {
            RelationOp op = static_cast<RelationOp>(i);
            if ((iter = find(restriction.begin(), restriction.end(), relationOps.find(op)->second)) !=
                restriction.end()) {
                relationOp = op;
            }
        }
        if (relationOp == invalidOp) {
            cerr << "Error: " << "yet unsupported operation." << endl;
            return vector<Restrict>(0);
        }
        string attrName{restriction.begin(), iter};
        attrName.erase(remove_if(attrName.begin(), attrName.end(), [](char x) { return isspace(x); }), attrName.end());

        string value{iter, restriction.end()};
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
                    case int_t:
                        int tmp_i;
                        if (valueStrIn >> tmp_i && !(valueStrIn >> tmp)) {
                            Restrict *intRestrict = new IntRestrict(attrName, relationOp, tmp_i);
                            restricts.push_back(*intRestrict);
                            delete (intRestrict);
                        } else {
                            cerr << "Error: " << " type not match!" << endl;
                        }
                        break;

                    case float_t:
                        float tmp_f;
                        if (valueStrIn >> tmp_f && !(valueStrIn >> tmp)) {
                            Restrict *floatRestrict = new FloatRestrict(attrName, relationOp, tmp_f);
                            restricts.push_back(*floatRestrict);
                            delete (floatRestrict);
                        } else {
                            cerr << "Error: " << " type not match!" << endl;
                        }
                        break;

                    case char_t:
                        string tmp_s;
                        if (valueStrIn >> tmp_s) {
                            if (*tmp_s.begin() == '\'' && *(tmp_s.end() - 1) == '\'') {
                                Restrict *stringRestrict =
                                        new StringRestrict(attrName, relationOp, tmp_s.substr(1, tmp_s.length() - 2));
                                restricts.push_back(*stringRestrict);
                                delete (stringRestrict);
                            }
                        } else {
                            cerr << "Error: " << " type not match!" << endl;
                        }
                        break;
                }
            } else {
                cerr << "Error: " << "No such attribute" << endl;
                return vector<Restrict>(0);
            }
        }
#endif
    }
    return restricts;
}

