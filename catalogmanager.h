#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H
#include "buffermanager.h"
#include "runtimeexception.h"
#include "table.h"

class catalogmanager
{
public:
    catalogmanager();
    void CreateTable(String Tablename, String Attributes);
    void DropTable(String Tablename);
    void DropIndex(String Indexname);
    void CreateIndex(String Indexname, String Tablename, String Attrname);
private:
    buffermanager buf;
    Table GetTable(String Tablename);
    void SetTable(const Table& tab);
};

#endif // CATALOGMANAGER_H
