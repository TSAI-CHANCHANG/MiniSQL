#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include "buffermanager.h"
#include "runtimeexception.h"
#include "table.h"

class catalogmanager
{
public:
    catalogmanager();
    void CreateTable(string Tablename, string Attributes);
    void DropTable(string Tablename);
    void DropIndex(string Indexname);
    void CreateIndex(string Indexname, string Tablename, string Attrname);
private:
    buffermanager buf;
    Table GetTable(string Tablename);
    void SetTable(const Table& tab);
};

#endif // CATALOGMANAGER_H
