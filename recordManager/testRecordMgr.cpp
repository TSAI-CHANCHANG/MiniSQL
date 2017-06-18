//
// Created by Zhao Xiaodong on 2017/6/6.
//

#include <iostream>
#include "RecordManager.h"
#include "../index+catalog+buffer/catalogmanager.h"

#define CREATE_TABEL 0
#define CREATE_TABEL_NO_INDEX 0
#define INSERT_VAL 1
#define INSERT_VAL_NO_INDEX 0
#define SELECT_REC 0
#define SELECT_REC_NO_INDEX 0
#define DELETE_REC 0


using namespace std;

int main(void) {
    BPLUSTREE BT(BLOCKSIZE);
    buffermanager bufferMgr;
    catalogmanager catalogMgr(bufferMgr);

#if CREATE_TABEL
    cout << catalogMgr.CreateTable("test_table", "t_id int, t_val int, primary key(t_id)", BT);
#endif

#if CREATE_TABEL_NO_INDEX
    cout << catalogMgr.CreateTable("test_table_no_index", "t_id int, t_val int", BT);
#endif

    RecordManager recordManager("test_table", catalogMgr, bufferMgr, BT);
//    RecordManager recordManagerNoIndex("test_table_no_index", catalogMgr, bufferMgr, BT);

#if INSERT_VAL
    recordManager.insertRecord("3, 101");
    recordManager.insertRecord("5, 102");
    recordManager.insertRecord("7, 103");
    recordManager.insertRecord("9, 101");
    recordManager.insertRecord("4, 101");
    recordManager.insertRecord("4, 105");
#endif
#if INSERT_VAL_NO_INDEX
    recordManagerNoIndex.insertRecord("3, 101");
    recordManagerNoIndex.insertRecord("5, 102");
    recordManagerNoIndex.insertRecord("7, 103");
    recordManagerNoIndex.insertRecord("9, 101");
    recordManagerNoIndex.insertRecord("4, 101");
    recordManagerNoIndex.insertRecord("4, 105");
#endif

#if DELETE_REC
    recordManager.deleteRecords("t_id >= 3 and t_id < 5 and t_val = 101");
#endif
    vector<string> attrs;
    attrs.push_back("t_val");
#if SELECT_REC
    recordManager.selectRecords(attrs, "t_id >= 3 and t_id <= 5");
#endif
#if SELECT_REC_NO_INDEX
    recordManagerNoIndex.selectRecords(attrs, "t_id >= 3 and t_id <= 5");
#endif
//    recordManager.deleteRecords("t_val = 101");
//    string whereClause1 = "a < 12 and b > 4\n and c = 'abc'";
//    recordManager.parseWhere(whereClause1);

    return 0;
}