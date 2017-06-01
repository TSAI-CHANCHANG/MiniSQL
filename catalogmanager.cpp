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
    table = blocks[buf.FindBlockinBuffer(Tablename+".tab",i)].content;//找到这个文件中的第一个block
    int pos1 = table.find(" ",0);
    int pos2 = table.find(" ",pos1+1);
    blocknum = atoi(table.substr(pos1+1,pos2-pos1-1).c_str());//文件中的第二项是blocknum，就是这个文件中一共有多少个block
    for (i=1;i<blocknum;i++)//将所有的block读进table这个字符串中
    {
        table+=blocks[buf.FindBlockinBuffer(Tablename+".tab",i)].content;
    }
    istrstream strin(table.c_str(),sizeof(table));//字符串输入流
    Table tab;
    string tmp;
    strin>>tmp>>tab.blocknum>>tab.primary;//读入tablename blocknum primary
    tab.setName(tmp);
    Attribute att;
    while (!strin.eof())//输入流结束之前，读入每一个attribute
    {
        strin>>att.attrname;
        strin>>att.type>>att.length>>att.primary>>att.unique>>att.indexname;
        tab.Attr.push_back(att);//将这个attribute存入
    }
    return tab;
}

void catalogmanager::DropTable(string Tablename)
{
    Table tab = GetTable(Tablename);//先找到这个表
    for (int i =0; i<tab.Attr.size();i++)
    {
        if (tab.Attr[i].indexname != "noindex")//如果某一个attribute上有index
        {
            //!!!调用indexmanager中的dropindex
            buf.DeleteFile(tab.Attr[i].indexname+".idx");//!!!不知道要不要写，不知道index manager写了没
        }
    }
    buf.DeleteFile(Tablename+".tab");//删表
}

void catalogmanager::DropIndex(string Indexname, string Tablename)
{
    Table tab = GetTable(Tablename);//找到这个index对应的表
    for (int i = 0; i<tab.Attr.size();i++)
        if (tab.Attr[i].indexname==Indexname)//找到这个index对应的attribute
        {
            buf.DeleteFile(tab.Attr[i].indexname+".idx");//!!!不知道要不要写，不知道index manager写了没
            tab.Attr[i].indexname = "noindex";//设为noindex
            break;
        }
    SetTable(tab);
}

//待定！！！！！
void catalogmanager::SetTable(const Table& tab)
{
    static char table[BLOCKSIZE];
    int i = 0;
    int blocknum = buf.FindBlockinBuffer(tab.getName()+".tab",i);//从tab中读入第一块，在buffermanager中，这一块的filename blocknumber都已经替换好了
    ostrstream strout(table,sizeof(table));//字符流输出
    strout<<tab.getName()<<" "<<tab.blocknum<<" "<<tab.primary;//将名字等基本信息输入字符流
    for (int j = 0;j<tab.Attr.size();j++)
    {
        if (strout.tellp()+sizeof(tab.Attr[j])>BLOCKSIZE)//如果这个block放不下了
        {
            strcpy(blocks[blocknum].content,table);//将字符流内容拷进来
            buf.DirtBlock(blocknum);//这个块已经被修改过了
            i++;
            blocknum = buf.FindBlockinBuffer(tab.getName()+".tab",i);//再找个块来
            strout.seekp(0);
        }
        strout<<" "<<tab.Attr[j].attrname<<" "<<tab.Attr[j].length<<" "<<tab.Attr[j].primary<<" "
              <<tab.Attr[j].unique<<" "<<tab.Attr[j].indexname;//将attribute的信息输入字符流
    }
    strcpy(blocks[blocknum].content,table);//最后一个块
    buf.DirtBlock(blocknum);
}

void catalogmanager::CreateIndex(string Indexname, string Tablename, string Attrname)
{
    Table tab = GetTable(Tablename);
    for (int i = 0; i<tab.Attr.size();i++)
        if (tab.Attr[i].attrname==Attrname)//找到对应表的对应attribute
        {
            tab.Attr[i].indexname = Indexname;
            break;
        }
    SetTable(tab);
}
