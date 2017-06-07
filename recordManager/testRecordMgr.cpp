//
// Created by Zhao Xiaodong on 2017/6/6.
//

#include <iostream>
#include "RecordManager.h"

using namespace std;

int main(void) {
    RecordManager recordManager("test");
    string whereClause1 = "a < 12";
    recordManager.parseWhere(whereClause1);
    return 0;
}