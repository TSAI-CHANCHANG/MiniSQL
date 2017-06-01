#include "catalogmanager.h"
#include <strstream>
#include<iostream>

#define MAXINT 32767;

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
    int pos1 = table.find(' ',0);
    int pos2 = table.find(' ',pos1+1);
    blocknum = atoi(table.substr(pos1+1,pos2-pos1-1).c_str());//文件中的第二项是blocknum，就是这个文件中一共有多少个block
    for (i=1;i<=blocknum;i++)//将所有的block读进table这个字符串中
    {
        table+=blocks[buf.FindBlockinBuffer(Tablename+".tab",i)].content;
    }
    istrstream strin(table.c_str(),sizeof(table));//字符串输入流
    Table tab;
    string tmp;
    strin>>tmp>>tab.blocknum>>tab.primary;//读入tablename blocknum primary
    tab.setName(tmp);
    Attribute att;
    int l = strin.tellg();
    strin.seekg(0, ios::end);
    l = strin.tellg()-l;//记录文件长度
    while (strin.tellg()<=l)//输入流结束之前，读入每一个attribute
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
            //!!!调用indexmanager中的dropindex   这个工作到底是这里干好还是api干好呢？
            //buf.DeleteFile(tab.Attr[i].indexname+".idx");//!!!不知道要不要写，不知道index manager写了没
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
void catalogmanager::SetTable(Table& tab)
{
    static char table[BLOCKSIZE];
    int i = 0;
    int blocknum = buf.FindBlockinBuffer(tab.getName()+".tab",i);//从tab中读入第一块，在buffermanager中，这一块的filename blocknumber都已经替换好了
    ostrstream strout(table,sizeof(table));//字符流输出
    strout<<tab.getName()<<".tab "<<tab.blocknum<<' '<<tab.primary;//将名字等基本信息输入字符流
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
        strout<<' '<<tab.Attr[j].attrname<<' '<<tab.Attr[j].type<<' '<<tab.Attr[j].length<<' '<<tab.Attr[j].primary<<' '
              <<tab.Attr[j].unique<<' '<<tab.Attr[j].indexname;//将attribute的信息输入字符流
    }
    tab.blocknum = i;
    strout.seekp(0);
    strout<<tab.getName()<<".tab "<<tab.blocknum<<' '<<tab.primary;//blocknum数量有误，确定之后重新输入一次
    strcpy(blocks[blocknum].content,table);//最后一个块
    cout<<blocks[blocknum].content;
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

int catalogmanager::CreateTable(string Tablename, string Attributes)
{
    Table tab;
    tab.setName(Tablename);
    Attribute att;
    Attributes+=',';
    int pos1, pos2;
    pos1 = 0;
    string Primarykey = "";
    int i = 0;
    while (1)
    {
        pos2 = Attributes.find(',',pos1);
        if (pos2 == -1) break;
        if (Attributes.substr(pos1, 7)!="primary")
        {
            att.primary = att.unique = false;
            att.defAtt(Attributes.substr(pos1, pos2-pos1));
            tab.Attr.push_back(att);
            if (att.primary) tab.primary = i;
            i++;
            pos1 = pos2+1;
        }
        else
        {
            pos1 = Attributes.find('(',pos1);
            pos2 = Attributes.find(')',pos1+1);
            Primarykey = Attributes.substr(pos1+1,pos2-pos1-1);
            pos1 = Attributes.find(',',pos2)+1;
        }
    }
    if (Primarykey!="")
        for (int i = 0; i<tab.Attr.size();i++)
            if (tab.Attr[i].attrname==Primarykey)//找到对应表的对应attribute
            {
                tab.Attr[i].primary = tab.Attr[i].unique = true;
                tab.primary = i;
                break;
            }
    tab.blocknum = 0;
    SetTable(tab);

    return tab.primary;
}

int Min(int a, int b)
{
    if (a == -1) a = MAXINT;
    if (b == -1) b = MAXINT;
    if (a>b) return b; else return a;
}

void Attribute::defAtt(string attDef)
{
    attDef+=' ';
    int pos1,pos2;//考虑多个空格
    pos1 = 0;
    while (attDef[pos1]==' '||attDef[pos1]=='\n') pos1++;
    if (attDef.find(' ',pos1)==-1&&attDef.find('\n',pos1)==-1) //报错
        ;
    pos2 = Min(attDef.find(' ',pos1),attDef.find('\n',pos1));//找不到报错
    this->attrname=attDef.substr(pos1,pos2-pos1);
    pos1 = pos2+1;
    while (attDef[pos1]==' '||attDef[pos1]=='\n') pos1++;
    if (attDef.find(' ',pos1)==-1&&attDef.find('\n',pos1)==-1&&attDef.find(',',pos1==-1)) //报错
        ;
    pos2 = Min(attDef.find(' ',pos1),attDef.find('\n',pos1));
    if (attDef.substr(pos1,3)=="int")
    {
        this->type = 1;
        this->length = 0;
    }
    else
        if (attDef.substr(pos1,5)=="float")
        {
            this->type = 2;
            this->length = 0;
        }
        else
            if (attDef.substr(pos1,4)=="char")
            {
                this->type = 3;
                int pos1 = attDef.find('(');
                int pos2 = attDef.find(')');
                this->length = atoi(attDef.substr(pos1+1,pos2-pos1-1).c_str());
            }
            else
            {
                //抛出错误
            }
    pos1 = pos2+1;
    while (pos1<=attDef.length()-1&&(attDef[pos1]==' '||attDef[pos1]=='\n')) pos1++;
    if (pos1>attDef.length()-1) return;
    pos2 = Min(attDef.find(' ',pos1),attDef.find('\n',pos1));
    if (attDef.substr(pos1,pos2-pos1)=="unique")
    {
        this->unique = true;
        return;
    }
    else if (attDef.substr(pos1,pos2-pos1)=="primary")
    {
        this->unique = this->primary = true;
        return;
    }
    else
    {
        //抛出错误
    }

    return;
}
