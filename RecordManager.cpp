//
// Created by Zhao Xiaodong on 2017/6/4.
//

#include <iostream>
#include <string>
#include <strstream>
#include "RecordManager.h"
#include "catalogmanager.h"
#include "table.h"

using namespace std;

bool RecordManager::insertRecord(string tableName, string rawValues) {
    catalogmanager catalogMgr;
    Table tableInfo = catalogMgr.GetTable(tableName);

    if (NO_SUCH_TABLE_ERROR) {
        // TODO: 没有该表如何
        cerr << "Error: " << "No such table named: " << tableName << endl;
    }

    string resultRecord;

    // TODO: not check space in string
    rawValues.erase(remove_if(rawValues.begin(), rawValues.end(), isspace), rawValues.end());
    istrstream strIn(rawValues.c_str(), rawValues.length());

    for (Attribute attr : tableInfo.Attr) {
        int tmp_i;
        float tmp_f;
        char tmp_c;
        string valueTerm;
        string tmp_s;

        getline(strIn, valueTerm, ','); // TODO: not check comma in string
        istrstream strTermIn(rawValues.c_str(), rawValues.length());
        switch (attr.type) {
            case int_t:
                strTermIn >> tmp_i; // TODO: check typeError
                break;
            case float_t:
                strTermIn >> tmp_f;
                break;
            case char_t:
                if (*valueTerm.begin() != '\'' || *(valueTerm.end() - 1) != '\'') {
                    cerr << "Error: " << "string input syntax error!\n" << valueTerm << endl;
                    return false;
                }
                valueTerm.erase(valueTerm.begin());
                valueTerm.erase(valueTerm.end());
                strTermIn >> valueTerm;
                break;
            default:
                break;
        }
        if (strTermIn.bad()) {
            cerr << "Error:  << "value not match error!" << endl;
        }
        if (!(strTermIn >> tmp_s)) {
            cerr << "Error: " << "value type syntax error!" << endl;
        }
    }
}
