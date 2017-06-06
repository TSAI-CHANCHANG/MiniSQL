//
// Created by Zhao Xiaodong on 2017/6/4.
//

#include <iostream>
#include <string>
#include <strstream>
#include "RecordManager.h"
#include "../catalogmanager.h"
#include "../buffermanager.h"
#include "../table.h"

#define DEBUG_IT true

using namespace std;

bool RecordManager::insertRecord(string rawValues) {
    catalogmanager catalogMgr;
    Table tableInfo = catalogMgr.GetTable(tableName);

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


    return true;
}

string RecordManager::generateInsertValues(string rawValues, Table tableInfo) {
    // TODO: check unique and primary
    string resultRecord;
    int tmp_i;
    float tmp_f;
    string tmp_s;

    // TODO: not check space in string
    rawValues.erase(remove_if(rawValues.begin(), rawValues.end(), isspace), rawValues.end());
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

        if ((*iter).unique) {
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
