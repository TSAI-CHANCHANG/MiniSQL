//
// Created by Zhao Xiaodong on 2017/6/6.
//

#include <iostream>
#include "RecordManager.h"
#include "../index+catalog+buffer/catalogmanager.h"

using namespace std;

int main(void) {
    catalogmanager catalogMgr;
    BPLUSTREE BT(BLOCKSIZE);
    buffermanager bufferMgr;

//    catalogMgr.CreateTable("test_table", "t_id int, t_val int, primary key(t_id)", BT);

    RecordManager recordManager("test_table", catalogMgr, bufferMgr, BT);

#if 0
    recordManager.insertRecord("3, 101");
    recordManager.insertRecord("5, 102");
    recordManager.insertRecord("7, 103");
    recordManager.insertRecord("9, 101");
    recordManager.insertRecord("4, 101");
    recordManager.insertRecord("4, 105");
#endif
//    cout << catalogMgr.CreateTable("testTable1", "t_id int,t_name   char(35) \n  unique ,   primary     key(t_id)", BT)
//         << endl;
//    RecordManager recordManager("testTable1");
    recordManager.deleteRecords("t_id >= 3 and t_id < 5 and t_val = 101");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    string whereClause1 = "a < 12 and b > 4\n and c = 'abc'";
//    recordManager.parseWhere(whereClause1);

    return 0;
}