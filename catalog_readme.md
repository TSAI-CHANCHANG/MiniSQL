#The Format of .tab Files
    tablename blocknum attrnum primary Attrname1 Attrtype1 attrlength1 primary1(bool) indexname1(string) unique1(bool)  Attrname2 Attrtype2бнбн    
##tablename
* the name of the table

##blocknum
* how much block the RECORD need in ".rec"file

##attrnum
* how much attributes in the table

##primary
* the position of the primary key, begin with 0 and -1 for no primary

##Attrname1
* the name of the first attribute

##attrlength1
* the length of the first attribute, int and float being 0, char(6) being 6

##primary1
* whether it is primary

##indexname1
* the indexname on the attribute, "noindex" for no index

##unique1
* whether it is unique

#Class Table

##int blocknum
* how many block in the RECORD need in ".rec" file

##short primary
* the position of the primary key, begin with 0 and -1 for no primary

##int Attrnum
* how much attributes in the table

##vector<Attribute> Attr
* a vector of the attributes

##string getName() const

##void setName(string tbname)

#Class Attribute
##short type
    #define TYPE_INT 1
    #define TYPE_FLOAT 2
    #define TYPE_CHAR 4

##short length
* it is meaningful only when type is TYPE_CHAR, its how long the char type is

##string attrname
* the attribute's name

##bool primary
* whether it is primary

##bool unique
* whether it is unique

##string indexname
* the name of the index on the attribute, no index is "noindex"

#GetTable
    Table GetTable(string Tablename);
##string Tablename
* the name of the table, the filename is tablename+".tab"

##Return Value
* An object of Table class

##Description
* Read the details about the table

#CreateTable
    int CreateTable(string Tablename, string Attributes,BPLUSTREE& BT);
##string Tablename
* the name of the table, offered by API

##string Attributes
* the string between brackets

##BPLUSTREE& BT
* the B+ tree of index manager, offered by API(There should be a BPLUSTREE class object in the API class)

##Returned Value
    #define HAVE_SUCH_TABLE 1002
    #define CONSTRAINT_DEFINATION_ERROR 1004
    #define WRONG_DEFINATION_FORMAT 1005
    #define CONSTRAINT_VIOLATION 1006
    #define SUCCESS 2333

##Description
* Receive the details and create the table
* divide the Attributes(string) into pieces by ','
* the one like "Name char(10) unique" is send to defAtt(string attDef) for further analyse
* the one like "primary key(ID)" is analysed here
* can deat with more than one blanks
* can detect almost all wrong formats,if there is sth wrong, return value is not SUCCESS
* after analysing, we set up a file and save all the details in it, using SetTable(Table tab)
* if it has a primary, the primary key's indexname is its attributename, and set an empty file of 'indexname.idx' and 'tablename.rec', build an empty B plus tree

#DropTable
    int DropTable(string Tablename, BPLUSTREE& BT)
##string Tablename
* name of the table

##BPLUSTREE& BT
* As above

##return value
    #define NO_SUCH_TABLE 1000
    #SUCCESS 2333
##Description
* first find whether the table has any index on it, if it has, then delete them using BPLUSTREE
* then delete the '.rec' and '.tab' files

#CreateIndex
    int CreateIndex(string Indexname, string Tablename, string Attrname, BPLUSTREE& BT)
##string Indexname
* the name of the index

## string Tablename
* the name of the table

## string Attrname
* the name of the Attrname

##BPLUSTREE& BT
* As above

##Returned Value
    #define NO_SUCH_TABLE 1000
    #define HAVE_SUCH_INDEX 1003
    #define CONSTRAINT_VIOLATION 1006
    #define SUCCESS 2333

##Description
* First, find the table and the attribute
* Second, set the tab.Attr[i].indexname = indexname;
* Then prepare for the Creation of BPLUSS Tree
 > read the records from '.rec'

 > get the record we need(the key in BPLUS tree)(using buffermanager)
 
 > write key block offset into the '.idx' file
 
 > using the '.idx' file to call BPLUSTREE CreaIndex

#DropIndex
    int DropIndex(string Indexname, string Tablename, BPLUSTREE& BT)
##string Indexname
* the index's name

##string Tablename
* the table the index is on

##BPLUSTREE& BT
* As above

##Returned Value
    #define NO_SUCH_TABLE 1000
    #define NO_SUCH_INDEX 1001
    #SUCCESS 2333

##Description
* Drop an index of a table
* can find some errors, e.g. no such index
* call the DropIndex in B Plus Tree(delete the ".idx" will be done by BPLUSTREE)
* turn the tab.attr[i].indexname into "noindex"


