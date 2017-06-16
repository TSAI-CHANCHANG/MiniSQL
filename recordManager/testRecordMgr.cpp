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

//    catalogMgr.CreateTable("test_table", "t_id int, t_val int, primary key(t_id)", BT);

    RecordManager recordManager("test_table");

#if 0
    recordManager.insertRecord(BT, "3, 101");
    recordManager.insertRecord(BT, "5, 102");
    recordManager.insertRecord(BT, "7, 103");
    recordManager.insertRecord(BT, "9, 101");
#endif
//    cout << catalogMgr.CreateTable("testTable1", "t_id int,t_name   char(35) \n  unique ,   primary     key(t_id)", BT)
//         << endl;
//    RecordManager recordManager("testTable1");
    recordManager.deleteRecords(BT, "t_id >= 3 and t_id < 5 and t_val = 101");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    recordManager.insertRecord(BT, "102, 'sd1'");
//    string whereClause1 = "a < 12 and b > 4\n and c = 'abc'";
//    recordManager.parseWhere(whereClause1);

    return 0;
}