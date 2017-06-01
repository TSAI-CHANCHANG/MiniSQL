#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include "buffermanager.h"
#include "runtimeexception.h"
#include "table.h"

class catalogmanager
{
public:
    catalogmanager();

    int CreateTable(string Tablename, string Attributes);
    //建表，返回值是primary的位置，如果没有就是-1，供API接下来建立索引用

    void DropTable(string Tablename);
    //删除表以及建立在这个表上的所有索引（这里的删除索引是指找到所有建立在这个表上的索引，调用index manager上的dropindex）

    void DropIndex(string Indexname, string Tablename);
    //删除索引（将这个index对应的表的对应的attribute上的indexname设为noindex）

    void CreateIndex(string Indexname, string Tablename, string Attrname);
    //建立索引（将这个index对应的表的对应的attribute上的indexname设为indexname）

private:
    buffermanager buf;

    Table GetTable(string Tablename);
    //通过缓存，从文件中获得一个表

    void SetTable(Table& tab);
    //通过缓存，将一个表存回文件中
};

#endif // CATALOGMANAGER_H
