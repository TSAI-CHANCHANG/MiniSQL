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
    istrstream strin(table.c_str(),table.length());//字符串输入流
    //cout<<sizeof(table)<<endl;
    Table tab;
    string tmp;
    strin>>tmp>>tab.blocknum>>tab.Attrnum>>tab.primary;//读入tablename blocknum primary
    tab.setName(tmp);

    for (i = 0;i<tab.Attrnum;i++)
    {
        Attribute att;
        strin>>att.attrname;
        strin>>att.type;
        strin>>att.length;
        strin>>att.primary;
        strin>>att.indexname;
        strin>>att.unique;
        tab.Attr.push_back(att);//将这个attribute存入
    }
    return tab;
}

int catalogmanager::DropTable(string Tablename)
{
    if (!buf.FindFile(Tablename+".tab")){
        //如果没有这个表，报错
        cout<<"No Such Table"<<endl;
        return -2;
    }
    Table tab = GetTable(Tablename);//先找到这个表
    for (int i =0; i<tab.Attrnum;i++)
    {
        if (tab.Attr[i].indexname != "noindex")//如果某一个attribute上有index
        {
            //!!!调用indexmanager中的dropindex   这个工作到底是这里干好还是api干好呢？
            buf.DeleteFile(tab.Attr[i].indexname+".idx");//!!!不知道要不要写，不知道index manager写了没
        }
    }
    buf.DeleteFile(Tablename+".tab");//删表
    return 0;
}

int catalogmanager::DropIndex(string Indexname, string Tablename)
{
    if (!buf.FindFile(Tablename+".tab")){
        cout<<"No Such Table"<<endl;
        return -2;
    }
    Table tab = GetTable(Tablename);//找到这个index对应的表
    int i;
    for (i = 0; i<tab.Attrnum;i++)
        if (tab.Attr[i].indexname==Indexname)//找到这个index对应的attribute
        {
            buf.DeleteFile(tab.Attr[i].indexname+".idx");//!!!不知道要不要写，不知道index manager写了没
            tab.Attr[i].indexname = "noindex";//设为noindex
            break;
        }
    if (i==tab.Attrnum){
        cout<<"No Such Index"<<endl;
        return -2;
    }
    SetTable(tab);
    return 0;
}

void catalogmanager::SetTable(Table& tab)
{
    static char table[BLOCKSIZE];
    int i = 0;
    int blocknum = buf.FindBlockinBuffer(tab.getName()+".tab",i);//从tab中读入第一块，在buffermanager中，这一块的filename blocknumber都已经替换好了
    ostrstream strout(table,BLOCKSIZE);//字符流输出
    strout<<tab.getName()<<" "<<tab.blocknum<<' '<<tab.Attrnum<<" "<<tab.primary;//将名字等基本信息输入字符流

    for (int j = 0;j<tab.Attrnum;j++)
    {
        int length = tab.Attr[j].attrname.length()+tab.Attr[j].indexname.length()+4;//估测的占用字节流的长度
        if (strout.tellp()+length>BLOCKSIZE)//如果这个block放不下了
        {
            strcpy(blocks[blocknum].content,table);//将字符流内容拷进来
            buf.DirtBlock(blocknum);//这个块已经被修改过了
            i++;
            blocknum = buf.FindBlockinBuffer(tab.getName()+".tab",i);//再找个块来
            strout.seekp(0);
        }
        strout<<' '<<tab.Attr[j].attrname<<' '<<tab.Attr[j].type<<' '<<tab.Attr[j].length<<' '<<tab.Attr[j].primary
             <<' '<<tab.Attr[j].indexname<<' '<<tab.Attr[j].unique;//将attribute的信息输入字符流
    }
    strcpy(blocks[blocknum].content,table);//最后一个块

    tab.blocknum = i;
    strout.seekp(0);
    strout<<tab.getName()<<" "<<tab.blocknum<<' '<<tab.Attrnum<<" "<<tab.primary;//blocknum数量有误，确定之后重新输入一次
    buf.DirtBlock(blocknum);
}

int catalogmanager::CreateIndex(string Indexname, string Tablename, string Attrname)
{
    if (!buf.FindFile(Tablename+".tab")){
        //已经存在这个表了
        cout<<"No Such Table"<<endl;
        return -2;
    }
    Table tab = GetTable(Tablename);//要建立index，先找到对应的表
    int i;
    for (i = 0; i<tab.Attrnum;i++)
        if (tab.Attr[i].attrname==Attrname)//找到对应表的对应attribute
        {
            tab.Attr[i].indexname = Indexname;
            break;
        }
    if (i==tab.Attrnum){
        //有这个表但没这个attribute
        cout<<"No Such Attribute"<<endl;
        return -2;
    }
    SetTable(tab);
    return 0;
}

int catalogmanager::CreateTable(string Tablename, string Attributes)//Attributes是括号中的内容
{
    Table tab;
    tab.setName(Tablename);//表名
    if (buf.FindFile(Tablename+".tab"))
    {
        //这个表已经存在了
        cout<<"Already exist!"<<endl;
        return -2;
    }
    Attribute att;
    Attributes+=',';//最后加个,方便处理
    int pos1, pos2;
    pos1 = 0;//用于切字串的定位符
    string Primarykey = "";//存储单独定义的primary属性
    int i = 0;

    while (1)
    {
        pos2 = Attributes.find(',',pos1);
        if (pos2 == -1) break;//找不到代表已经到了末尾了

        //如果不是单独定义的primary，就是普通的一个属性的定义
        if (Attributes.substr(pos1, 7)!="primary")
        {
            //初始化定义
            tab.Attrnum++;
            att.attrname = "";att.type = -1;
            att.primary = att.unique = false;

            //处理两个,中间的这个串
            att.defAtt(Attributes.substr(pos1, pos2-pos1));

            if (att.type == -1) return -2;//格式出错，返回的att.type=-1，于是报错，返回
            tab.Attr.push_back(att);//存起来
            if (att.primary)//如果这个属性是个primary
                if (tab.primary!=-1)//之前已经定义过primary重复，报错
                {
                     cout<<"Two primary keys!"<<endl;
                     return -2;
                }
                else
                    tab.primary = i;//这个表的primary确定
            else
            {
            }
            i++;
            pos1 = pos2+1;
            while (Attributes[pos1]==' '||Attributes[pos1]=='\n') pos1++;//除去多余空格

        }
        else
        {   //单独定义的primary
            if (tab.primary!=-1) {

                cout<<"Two primary keys!"<<endl;
                return -2;
            }
            pos1 = Attributes.find('(',pos1);
            pos2 = Attributes.find(')',pos1+1);
            Primarykey = Attributes.substr(pos1+1,pos2-pos1-1);
            pos1 = Attributes.find(',',pos2)+1;
        }
    }
    if (Primarykey!="")//单独定义的primary需要定义在表格里
    {
        for (int i = 0; i<tab.Attrnum;i++)
            if (tab.Attr[i].attrname==Primarykey)//找到对应表的对应attribute
            {
                tab.Attr[i].primary = tab.Attr[i].unique = true;
                tab.primary = i;
                break;
            }
        if (tab.primary==-1)
        {
            cout<<"No Such Attribute!"<<endl;
            return -2;
        }
    }
    tab.blocknum = 0;//先定义为0，settable过程中找
    SetTable(tab);//将这个表的内容存回文件中

    return tab.primary;//返回主键位置（第几个attribute），没有就是-1
}

int Min(int a, int b)//在找得到的两个位置里找最小的
{
    if (a == -1) a = MAXINT;
    if (b == -1) b = MAXINT;
    if (a>b) return b; else return a;
}

void Attribute::defAtt(string attDef)//处理表的属性定义
{
    attDef+=' ';
    int pos1,pos2;//考虑多个空格
    pos1 = 0;

    //处理空格和格式错误
    while (pos1<=attDef.length()-1&&attDef[pos1]==' '||attDef[pos1]=='\n') pos1++;
    if ((pos1>attDef.length()-1)||attDef.find(' ',pos1)==-1&&attDef.find('\n',pos1)==-1)
    {
        cout<<"Wrong Format"<<endl;
        return;
    }

    pos2 = Min(attDef.find(' ',pos1),attDef.find('\n',pos1));
    this->attrname=attDef.substr(pos1,pos2-pos1);//第一段有意义的字符串是attributename

    pos1 = pos2+1;
    //处理空格和格式错误
    while (pos1<=attDef.length()-1&&(attDef[pos1]==' '||attDef[pos1]=='\n')) pos1++;
    if ( (pos1>attDef.length()-1)||attDef.find(' ',pos1)==-1&&attDef.find('\n',pos1)==-1) //报错
    {
        cout<<"Wrong Format"<<endl;
        return;
    }

    pos2 = Min(attDef.find(' ',pos1),attDef.find('\n',pos1));
    //第二个字符串是类型
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
                //不属于三种类型，错误
                cout<<"Wrong Type!"<<endl;
                return;
            }

    pos1 = pos2+1;
    //处理末尾空格
    while (pos1<=attDef.length()-1&&(attDef[pos1]==' '||attDef[pos1]=='\n')) pos1++;
    if (pos1>attDef.length()-1) return;

    //如果后面还有有意义的字符串，就是约束
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
        cout<<"Wrong Constraint!"<<endl;
        this->type = -1;
        return;
    }

    return;
}
