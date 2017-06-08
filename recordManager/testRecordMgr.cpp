//
// Created by Zhao Xiaodong on 2017/6/6.
//

#include <iostream>
#include "RecordManager.h"
#include "../record+catalog+buffer/catalogmanager.h"

using namespace std;

int main(void) {
    catalogmanager catalogMgr;
    BPLUSTREE BT(BLOCKSIZE);
    cout << catalogMgr.CreateTable("testTable1", "t_id int,t_name   char(35) \n  unique ,   primary     key(ID)", BT)
         << endl;
//    RecordManager recordManager("testTable1");
//    recordManager.insertRecord("101, 'zxd'");
//    string whereClause1 = "a < 12 and b > 4\n and c = 'abc'";
    return 0;
}