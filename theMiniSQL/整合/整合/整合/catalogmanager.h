#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include "buffermanager.h"
#include "IndexManager.h"
#include "table.h"
#include "defination.h"



class catalogmanager
{
public:
    catalogmanager(buffermanager& buff):buf(buff),ErrorInfo(SUCCESS){
    }

    int CreateTable(string Tablename, string Attributes,BPLUSTREE& BT);
    //建表，primary理应有一个同名的index，但因为目前没有数据，所以要等record第一次插入的时候再建立索引

    int DropTable(string Tablename,BPLUSTREE& BT);
    //删除表以及建立在这个表上的所有索引（这里的删除索引是指找到所有建立在这个表上的索引，调用index manager上的dropindex）
    //用B+树调用删索引功能，删除对应的三个文件：这个表的catalog、表中的内容、表上的索引

    int DropIndex(string Indexname, string Tablename,BPLUSTREE& BT);
    //删除索引（将这个index对应的表的对应的attribute上的indexname设为noindex）
    //用B+树调用删索引功能

    int CreateIndex(string Indexname, string Tablename, string Attrname,BPLUSTREE& BT);
    //建立索引（将这个index对应的表的对应的attribute上的indexname设为indexname）
    //调用B+树的建立索引（默认filename为表名.rec）

    Table GetTable(string Tablename);
    //通过缓存，从文件中获得一个表

    void SetTable(Table& tab);
    //通过缓存，将一个表存回文件中

    void PrepareForIndex(string FromFile, string ToFile, int tpnum, const Table& tab);

    short ErrorInfo;

private:
    buffermanager& buf;

    void CreateEmptyFile(string fileName);

};

#endif // CATALOGMANAGER_H
