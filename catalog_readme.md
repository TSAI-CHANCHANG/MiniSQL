# .tab文件的格式

```
tablename blocknum attrnum primary Attrname1 Attrtype1 attrlength1 primary1(bool) indexname1(string) unique1(bool)  Attrname2 Attrtype2……    
```

## tablename

*   表名

## blocknum

*   RECORD在“.rec”文件中需要多少块

## attrnum

*   表中有多少属性

## primary

*   主键的位置，从0开始，-1为无主键

## Attrname1

*   第一个属性的名称

## attrlength1

*   第一个属性的长度，int和float为0，char（6）为6

## primary1

*   是否是主键

## indexname1

*   属性上的indexname，没有索引的就是“noindex”

## unique1

*   是否是独一无二的

# Class Table

## int blocknum

*   RECORD中有多少块需要在“.rec”文件中

## short primary

*   主键的位置从0开始，-1为无主键

## int Attrnum

*   表中有多少属性

## vector <attribute>Attr</attribute>

*   属性的向量

## string getName() const

## void setName(string tbname)

# Class Attribute

## short type

```
#define TYPE_INT 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 4
```

## short length

*   只有当type是TYPE_CHAR时，它才有意义，它的char类型是多长

## string attrname

*   属性的名称

## bool primary

*   是否是主键

## bool unique

*   是否唯一

## string indexname

*   该属性的索引名称，无索引为“noindex”

# GetTable

```
Table GetTable(string Tablename);
```

## string Tablename

*   表的名称，文件名是tablename +'.tab'

## Return Value

*   表类的一个对象

## Description

*   读入有关表格的详细信息

# CreateTable

```
int CreateTable(string Tablename, string Attributes,BPLUSTREE& BT);
```

## string Tablename

*   由API提供的表的名称

## string Attributes

*   括号之间的字符串

## BPLUSTREE& BT

*   索引管理器的B +树，由API提供（API类中应该有一个BPLUSTREE类对象）

## Returned Value

```
#define HAVE_SUCH_TABLE 1002
#define CONSTRAINT_DEFINATION_ERROR 1004
#define WRONG_DEFINATION_FORMAT 1005
#define CONSTRAINT_VIOLATION 1006
#define SUCCESS 2333
```

## Description

*   收到细节并创建表
*   将属性（字符串）以','划分
*   像“Name char（10）unique”一样被发送到defAtt（string attDef）进一步分析
*   这里分析一个像“primary key（ID）”一样的字符串
*   可以多于一个空白
*   可以检测几乎所有错误的格式，如果有错误，返回值不是SUCCESS
*   分析后，我们设置一个文件并保存所有的细节，使用SetTable（Table tab）
*   如果它有一个主键，主键的索引名称是其属性名称，并设置一个空的文件'indexname.idx'和'tablename.rec'，构建一个空的B加树

# DropTable

```
int DropTable(string Tablename, BPLUSTREE& BT)
```

## string Tablename

*   表名

## BPLUSTREE& BT

*   同上

## return value

```
#define NO_SUCH_TABLE 1000
#SUCCESS 2333
```

## Description

*   首先找到表上是否有任何索引，如果它有，然后使用BPLUSTREE删除它们
*   然后删除'.rec'和'.tab'文件

# CreateIndex

```
int CreateIndex(string Indexname, string Tablename, string Attrname, BPLUSTREE& BT)
```

## string Indexname

*   索引名

## string Tablename

*   表名

## string Attrname

*   表上的属性名

## BPLUSTREE& BT

*   同上

## Returned Value

```
#define NO_SUCH_TABLE 1000
#define HAVE_SUCH_INDEX 1003
#define CONSTRAINT_VIOLATION 1006
#define SUCCESS 2333
```

## Description

*   首先，找到表和属性
*   其次，设置tab.Attr [i] .indexname = indexname;
*   然后准备创建BPLUSS树，包括如下工程

    > 从'.rec'读取记录（用buffer manager）
    > 
    > 获取我们需要的记录（BPLUS树中的关键字）
    > 
    > 将key block offset写入'.idx'文件
    > 
*   使用'.idx'文件调用BPLUSTREE CreateIndex

# DropIndex

```
int DropIndex(string Indexname, string Tablename, BPLUSTREE& BT)
```

## string Indexname

*   索引名

## string Tablename

*   索引所在的表

## BPLUSTREE& BT

*   同上

## Returned Value

```
#define NO_SUCH_TABLE 1000
#define NO_SUCH_INDEX 1001
#SUCCESS 2333
```

## Description

*   删除一个表的索引
*   可以发现一些错误，例如 没有这样的索引
*   在B Plus Tree中调用DropIndex（删除“.idx”将由BPLUSTREE完成）
*   将tab.attr [i] .indexname转换为“noindex”