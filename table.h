#ifndef TABLE_H
#define TABLE_H
#include<vector>
using namespace std;

class Attribute
{
public:
    short type;//三种类型int 1, float 2, char 3
    String attrname;
    bool primary;//这个属性是否是primary
    bool unique;//这个属性是否是unique
};

class Table
{
public:
    int blocknum;//这个表的文件里一共占多少个block
    short primary;//primary的位置，-1代表没有
    vector<tuples*> record;//一个tuple是指一行的数据内容，这个我得catalog管不着，留给record manager了
    String getName(){
        return tablename;
    }

    Table();
    Table (String tbname, vector<Attribute> Att){
        tablename = tbname;
        Attr = Att;
    }

private:
    String tablename;//表的名字
    vector<Attribute> Attr;//表的属性栏
};

#endif // TABLE_H
