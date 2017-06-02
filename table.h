#ifndef TABLE_H
#define TABLE_H
#include<vector>
#include<string.h>
using namespace std;

class Attribute
{
public:
    short type;//三种类型int 1, float 2, char 3
    short length;//只有char才有意义， 长度为定义的大小
    string attrname;
    bool primary;//这个属性是否是primary
    bool unique;//这个属性是否是unique
    string indexname;//这个属性的indexname，没有就是"noindex"
    void defAtt(string attDef);
    Attribute():primary(false),unique(false),indexname("noindex"){}
};

class Table
{
public:
    int blocknum;//这个表的文件里一共占多少个block，不到1个算0个
    short primary;//primary的位置，-1代表没有
    //vector<tuples*> record;//一个tuple是指一行的数据内容，这个我得catalog管不着，留给record manager了
    int Attrnum;
    vector<Attribute> Attr;//表的属性栏
    string getName() const
    {
        return tablename;
    }
    void setName(string tbname)
    {
        tablename = tbname;
    }

    Table():primary(-1),Attrnum(0){}
    Table (string tbname, vector<Attribute> Att){
        tablename = tbname;
        Attr = Att;
    }

private:
    string tablename;//表的名字

};
//这个表的catalog在文件中应该是：
//tablename blocknum attrnum primary Attrname1 Attrtype1 attrlength1 primary1(bool) unique1(bool) index1(bool) Attrname2 Attrtype2……


#endif // TABLE_H
