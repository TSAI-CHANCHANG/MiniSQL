#include "catalogmanager.h"
#include <strstream>

extern block blocks[BLOCKNUMBER];

catalogmanager::catalogmanager()
{

}

Table catalogmanager::GetTable(String Tablename)
{
    int i = 0;
    int blocknum;
    string table;
    table = blocks[buf.FindBlockinBuffer(Tablename+".tab",i)];
    istrstream strin(table,sizeof(table));
    string tmp;
    strin>>tmp;
    Table thistable;
    thistable.setName(tmp);
    strin>>thistable.blocknum>>thistable.primary;
    Attribute att;
    while (i<thistable.blocknum)
    {
        if (strin.tellg()>=blocks[blocknum].usedsize){
            i++;
            if (i==thistable.blocknum) break;
            table = blocks[buf.FindBlockinBuffer(Tablename+".tab",i)];
            strin(table,sizeof(table));
        }
        strin>>att.attrname;
        strin>>att.type>>att.length>>att.primary>>att.unique>>att.index;
        thistable.Attr.push_back(att);
    }
    return thistable;
}

void catalogmanager::DropTable(String Tablename)
{
    //删掉所有与之相关的index，还没写，等待和index manager沟通
    buf.DeleteFile(Tablename+".tab");
}

void catalogmanager::DropIndex(String Indexname)
{
    string tablename;
    string attributename;
    //我找到了这个table和它的attributename 还没写，等待和index manager沟通
    Table tab = GetTable(tablename);
    for (int i = 0; i<tab.Attr.size();i++)
        if (tab.Attr[i].attrname==attributename)
        {
            tab.Attr[i].index = false;
            break;
        }
    SetTable(tab);
}

void catalogmanager::SetTable(const Table& tab)
{
    buf.FindBlockinBuffer(tab.getName(),0);
    string table;
    int i = 0;
    int blocknum = buf.FindBlockinBuffer(Tablename+".tab",i);
    table = blocks[blocknum];
    ostrstream strout(table,sizeof(table));
    strout<<tab.getName()<<" "<<tab.blocknum<<" "<<tab.primary;
    for (int j = 0;j<tab.Attr.size();j++)
    {
        if (strout.tellp()+sizeof(tab.Attr[j])>BLOCKSIZE)
        {
            blocks[blocknum]=table.c_str();
            buf.DirtBlock(blocknum);
            i++;
            blocknum = buf.FindBlockinBuffer(Tablename+".tab",i);
            table = blocks[blocknum];
            strout(table,sizeof(table));
        }
        strout<<" "<<tab.Attr[j].attrname<<" "<<tab.Attr[j].length<<" "<<tab.Attr[j].primary<<" "
              <<tab.Attr[j].unique<<" "<<tab.Attr[j].index;
    }
    blocks[blocknum]=table.c_str();
    buf.DirtBlock(blocknum);
}

void catalogmanager::CreateIndex(String Indexname, String Tablename, String Attrname)
{
    Table tab = GetTable(Tablename);
    for (int i = 0; i<tab.Attr.size();i++)
        if (tab.Attr[i].attrname==Attrname)
        {
            tab.Attr[i].index = true;
            break;
        }
    SetTable(tab);
}
