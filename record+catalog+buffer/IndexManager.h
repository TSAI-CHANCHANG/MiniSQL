#pragma once
/*	Need block_size when construct.
CreateIndex:
	Need a file have key and a integer(address of offset) every line.
	Transfer the name of attribute ,file an type to me.
	
*/
#include <map>
#include <vector>
#include "defination.h"
/*
#ifndef INDEX_H
#define INDEX_H
#define POINTERSIZE 4
#define TYPE_INT 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 4
#define SUCCESS 233
#define ERROR_TYPE -1
#define ERROR_FILE -2
#define ERROR_INDEX_EXISTS -3
#define ERROR_OFFSET_NOT_EXISTS -4
#define ERROR_UNKNOWN -233
The code above should be added in api
#endif*/


#define POINTERSIZE 4

using namespace std;

int GetDegree(int block_size, int type);

struct INTNODE
{
	vector<int> key;
	vector<INTNODE*> pointer;
	vector<int> block, offset;
	INTNODE *father, *prev, *next;
};
struct FLOATNODE
{
	vector<float> key;
	vector<FLOATNODE*> pointer;
	vector<int> block, offset;
	FLOATNODE *father, *prev, *next;
};
struct CHARNODE
{
	vector<string> key;
	vector<CHARNODE*> pointer;
	vector<int> block, offset;
	CHARNODE *father, *prev, *next;
};


class BPLUSTREE
{
public:
	BPLUSTREE(int block_size);
	~BPLUSTREE();
	
	int CreateIndex(string *file_name, int type);
	int DropIndex(string *file_name, int type);
	int Insert(int type, string *file_name, string *key, int block, int offset);
	int Delete(int type, string *file_name, string *lbound, string *rbound, int lopen, int ropen);
	int Find(int type, string *file_name, string *lbound, string *rbound, int lopen, int ropen);
	int DeleteNode(int type, string *file_name, string *skey);


private:
	int block_size;
	map<string, INTNODE*> int_map;
	map<string, FLOATNODE*> float_map;
	map<string, CHARNODE*> char_map;
	int AddNode(int type, string *file_name, string *key, int block, int offset);
	int CreateTree(int type, string *file_name);
	//int DeleteNode(int type, string *file_name, string *skey);
};

