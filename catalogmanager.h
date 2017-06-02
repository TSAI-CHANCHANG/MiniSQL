#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include "buffermanager.h"
#include "IndexManager.h"
#include "table.h"
#include "defination.h"



class catalogmanager
{
public:
    catalogmanager(){}

    int CreateTable(string Tablename, string Attributes,BPLUSTREE& BT);
    //建表，把indexmanager的那个B+树塞进来，顺带着一起处理了，下面都是这样

    int DropTable(string Tablename,BPLUSTREE& BT);
    //删除表以及建立在这个表上的所有索引（这里的删除索引是指找到所有建立在这个表上的索引，调用index manager上的dropindex）

    int DropIndex(string Indexname, string Tablename,BPLUSTREE& BT);
    //删除索引（将这个index对应的表的对应的attribute上的indexname设为noindex）

    int CreateIndex(string Indexname, string Tablename, string Attrname,BPLUSTREE& BT);
    //建立索引（将这个index对应的表的对应的attribute上的indexname设为indexname）

    Table GetTable(string Tablename);
    //通过缓存，从文件中获得一个表

private:
    buffermanager buf;

    void SetTable(Table& tab);
    //通过缓存，将一个表存回文件中
};

#endif // CATALOGMANAGER_H
