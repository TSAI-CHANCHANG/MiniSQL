#include "catalogmanager.h"
#include <strstream>

extern block blocks[BLOCKNUMBER];

catalogmanager::catalogmanager()
{

}

Table catalogmanager::GetTable(string Tablename)
{
    int i = 0;
    int blocknum;
    string table;
    table = blocks[buf.FindBlockinBuffer(Tablename+".tab",i)].content;
    int pos1 = table.find(" ",0);
    int pos2 = table.find(" ",pos1+1);
    blocknum = atoi(table.substr(pos1+1,pos2-pos1-1).c_str());
    for (i=1;i<blocknum;i++)
    {
        table+=blocks[buf.FindBlockinBuffer(Tablename+".tab",i)].content;
    }
    istrstream strin(table.c_str(),sizeof(table));
    Table tab;
    string tmp;
    strin>>tmp>>tab.blocknum>>tab.primary;
    tab.setName(tmp);
    Attribute att;
    while (!strin.eof())
    {
        strin>>att.attrname;
        strin>>att.type>>att.length>>att.primary>>att.unique>>att.indexname;
        tab.Attr.push_back(att);
    }
    return tab;
}

void catalogmanager::DropTable(string Tablename)
{
    //删掉所有与之相关的index，还没写，等待和index manager沟通
    buf.DeleteFile(Tablename+".tab");
}

void catalogmanager::DropIndex(string Indexname)
{
    string tablename;
    string attributename;
    //我找到了这个table和它的attributename 还没写，等待和index manager沟通
    Table tab = GetTable(tablename);
    for (int i = 0; i<tab.Attr.size();i++)
        if (tab.Attr[i].attrname==attributename)
        {
            tab.Attr[i].indexname = "noindex";
            break;
        }
    SetTable(tab);
}

void catalogmanager::SetTable(const Table& tab)
{
    static char table[BLOCKSIZE];
    int i = 0;
    int blocknum = buf.FindBlockinBuffer(tab.getName()+".tab",i);
    ostrstream strout(table,sizeof(table));
    strout<<tab.getName()<<" "<<tab.blocknum<<" "<<tab.primary;
    for (int j = 0;j<tab.Attr.size();j++)
    {
        if (strout.tellp()+sizeof(tab.Attr[j])>BLOCKSIZE)
        {
            strcpy(blocks[blocknum].content,table);
            buf.DirtBlock(blocknum);
            i++;
            blocknum = buf.FindBlockinBuffer(tab.getName()+".tab",i);
            strout.seekp(0);
        }
        strout<<" "<<tab.Attr[j].attrname<<" "<<tab.Attr[j].length<<" "<<tab.Attr[j].primary<<" "
              <<tab.Attr[j].unique<<" "<<tab.Attr[j].indexname;
    }
    strcpy(blocks[blocknum].content,table);
    buf.DirtBlock(blocknum);
}

void catalogmanager::CreateIndex(string Indexname, string Tablename, string Attrname)
{
    Table tab = GetTable(Tablename);
    for (int i = 0; i<tab.Attr.size();i++)
        if (tab.Attr[i].attrname==Attrname)
        {
            tab.Attr[i].indexname = Indexname;
            break;
        }
    SetTable(tab);
}
