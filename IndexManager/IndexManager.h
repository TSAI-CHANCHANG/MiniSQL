#pragma once
/*	Need block_size when construct.
CreateIndex:
	Need a file have key and a integer(address of offset) every line.
	Transfer the name of attribute ,file an type to me.
	
*/
#include <map>
#include <vector>

#define POINTERSIZE 4
/*The foollowing code should be added in api*/
#define TYPE_INT 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 4
#define SUCCESS 233
#define ERROR_TYPE -1
#define ERROR_FILE -2
#define ERROR_INDEX_EXISTS -3
#define ERROR_offset_NOT_EXISTS -4
#define ERROR_UNKNOWN -233
/*The code above should be added in api */

using namespace std;

int GetDegree(int block_size, int type);

struct INTNODE
{
	vector<int> key;
	vector<INTNODE*> pointer;
	vector<int> offset, block;
	INTNODE *father;
};
struct FLOATNODE
{
	vector<float> key;
	vector<FLOATNODE*> pointer;
	vector<int> offset;
	FLOATNODE *father;
};
struct CHARNODE
{
	vector<char*> key;
	vector<CHARNODE*> pointer;
	vector<int> offset;
	CHARNODE *father;
};


class BPLUSTREE
{
public:
	BPLUSTREE(int block_size);
	~BPLUSTREE();
	int CreateTree(int type, string *file_name);
	int CreateIndex(string *attribute, string *file_name, int type);
	int DropIndex(string *file_name);
	int AddNode(int type, string *file_name, string *key, int block, int offset);
	int DeleteNode(int type, string *file_name, string *skey);


private:
	int block_size;
	map<string, string> index_to_file;
	map<string, INTNODE*> int_map;
	map<string, FLOATNODE*> float_map;
	map<string, CHARNODE*> char_map;
};

