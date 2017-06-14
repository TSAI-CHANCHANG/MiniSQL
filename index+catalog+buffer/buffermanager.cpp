#include <iostream>
#include <fstream> 
#include<cstdio>
#include<string.h>

#include"defination.h"
#include "buffermanager.h"
#include "catalogmanager.h"

block blocks[BLOCKNUMBER];//预计有10个buffer中的block

short flag;

void block::ClearBlock(){
    for (int i=0;i<BLOCKSIZE+1;i++) content[i]=0;
    filename = "";
    dirty = pin = used = false;
    usedsize = 0;
    fileoffset = 0;
}

void block::SetBlock(string filename, int fileoffset)
{
    this->filename = filename;
    this->fileoffset = fileoffset;
    dirty = false;
    pin = false;
    used = true;
    usedsize = FindUsedSize();
    Recenttime = time(NULL);
}

int block::FindUsedSize()
{
    for (int i = 0; i< BLOCKSIZE; i++)
        if (content[i]=='\0') return i;
    return BLOCKSIZE;
}

buffermanager::buffermanager()
{
    for (int i = 0; i<BLOCKNUMBER;i++)
        blocks[i].ClearBlock();
}

buffermanager::~buffermanager()
{
    for (int i = 0; i<BLOCKNUMBER; i++)
        WriteBack(i);
}

char* buffermanager::GetDetail(int blocknum, int blockoffset)
{
    return blocks[blocknum].content+blockoffset;
}

int buffermanager::WriteBack(int blocknum)
{
    //将一个块写回文件
    if (!blocks[blocknum].dirty) return SUCCESS;
    const char* filename = blocks[blocknum].filename.c_str();
    fstream File(filename);
    if (!File)
    {
        cout<<"File Open Failure!"<<endl;
        return FILE_ERROR;
    }

    File.seekp(blocks[blocknum].fileoffset*BLOCKSIZE,ios::beg);

    //cout<<File.tellp()<<endl;
    File<<blocks[blocknum].content;
    //cout<<strlen(blocks[blocknum].content)<<endl;
    //cout<<File.tellp()<<endl;
    File.close();
    //清除这个块的相关信息

    blocks[blocknum].ClearBlock();

    return SUCCESS;
}

int buffermanager::FindBlockinBuffer(string fileName, int offset)
{
    flag = 0;
    //如果这个块已经在buffer中，返回这个块在buffer中的number
    for (int i = 0; i< BLOCKNUMBER; i++)
        if (blocks[i].used&& blocks[i].filename==fileName && blocks[i].fileoffset==offset)
        {
            UpdateBlock(i);
            return i;
        }

    //如果不在，就先找buffer中一个空的块，然后将这个块从文件里写入buffer
    int blocknum = GetAnEmptyBlock();
    if (WriteIn(fileName,offset,blocknum)==NOT_ENOUGH) flag = NO_SUCH_BLOCK;
    return blocknum;
}

int buffermanager::WriteIn(string fileName, int offset, int blocknum)
{
    //将这个块的内容从文件中写入
    const char* filename = fileName.c_str();
    ifstream File(filename, std::ios::in|ios::app);
    if (!File.is_open())
    {
        cout<<"File Open Failure!"<<endl;
        return FILE_ERROR;
    }

    blocks[blocknum].SetBlock(fileName, offset);

    File.seekg(0, ios::beg);
    int l = (int)File.tellg();
    File.seekg(0, ios::end);
    l = (int)File.tellg()-l;//记录文件长度
    if (l<offset*BLOCKSIZE)
    {
        File.close();
        return NOT_ENOUGH;
    }
    File.clear();

    File.seekg(offset*BLOCKSIZE, ios::beg);

    int i;
    for (i = 0; i<BLOCKSIZE&&!File.eof(); i++)
    {
        File.get(blocks[blocknum].content[i]);
    }

    File.close();
    //if (i<BLOCKSIZE) WriteBack(blocknum);
    //set这个块的相关信息，包括对应文件和对应文件中的offset、最近修改时间等等

    return SUCCESS;
}

int buffermanager::GetAnEmptyBlock()
{
    //如果buffer中有空的快，直接返回
    for (int i = 0; i < BLOCKNUMBER; i++)
        if (blocks[i].used==false)
            return i;

    //按照LRU找一个最近没用过且没有被固定的块
    int LRU = 0;
    for (int i = 1; i< BLOCKNUMBER; i++)
        if (!blocks[i].pin && blocks[i].Recenttime<blocks[LRU].Recenttime)
            LRU = i;
    //如果这个块是dirty的，那么要先写回文件中
    if (blocks[LRU].dirty) WriteBack(LRU); else blocks[LRU].ClearBlock();
    return LRU;
}

int Space(char* ch, int size)
{
    int pos = 0;
    while (*ch)
    { 
        int i = 0;
        while (*ch&&(*ch==' '||*ch == 10)) {
            ch++;
            i++;
        }

        if (i>=size*2) return pos;

        if (!*ch) break;
        pos = pos+i;

        while (*ch&&*ch!=' '&&*ch != 10){
            ch++;
            pos++;
        }
    }
    if (pos+size*3<BLOCKSIZE-500) return pos; else return -1;
}

void buffermanager::FindSuitBlockinBuffer(string fileName, int size, int* blocknum, int * blockoffset)
{
    //如果buffer里有可以用的块，直接用
    for (int i = 0; i< BLOCKNUMBER; i++){
        if (blocks[i].used&& blocks[i].filename==fileName)
            if ((*blockoffset=Space(blocks[i].content,size)+1)!=0)
        {
                //cout<<*blockoffset<<endl;
            UpdateBlock(i);
            blocks[i].usedsize+=size;
            *blocknum = i;
            return ;
            }
    }

    int i = 0;
    while(1)
    {
        *blocknum = FindBlockinBuffer(fileName,i);
        //cout<<blocks[*blocknum].content<<endl;
        if (flag==NO_SUCH_BLOCK){
            ofstream File(fileName.c_str(),ios::app);
            File.seekp(i*BLOCKSIZE, ios::beg);
            File<<" ";
            File.close();
            *blockoffset = 0;
            return;
        }
        if (!blocks[*blocknum].content[0]) {
            *blockoffset = 0;
            return;
        }
        if ((*blockoffset = Space(blocks[*blocknum].content,size)+1)!=0) return;
        i++;
    }

    //version 1.0
    //没有可以直接用的块，就去文件里一个块一个块地找
    /*
    const char* filename = fileName.c_str();
    fstream File(filename);
    int l = File.tellg();
    File.seekg(0, ios::end);
    l = File.tellg()-l;//记录文件长度
    File.seekg(0, ios::beg);

    char ch;
    long count = 0;
    while (1)
    {
        File >> ch;
        while (ch&&!File.eof())
        {
            File >> ch;
            count++;
        }
        int d = (count / BLOCKSIZE + 1) * BLOCKSIZE - count;
        if (d>=size) {//有一个块的剩余空间足够插入size大小的数据
            File.close();
            int blocknum = GetAnEmptyBlock();//将它写进buffer中去
            WriteIn(fileName,count/BLOCKSIZE,blocknum);
            return blocknum;
        }
        else
        {
            count = (count / BLOCKSIZE + 1) * BLOCKSIZE;
        }
        if (count>=l) break;
    }
    //文件中目前没有合适的块，需要新增一个块
    File.seekg(count, ios::beg);
    File<<" ";
    File.close();
    int blocknum = GetAnEmptyBlock();//将它写进buffer中去
    WriteIn(fileName,count/BLOCKSIZE,blocknum);*/

}

void buffermanager::DirtBlock(int blocknum)
{
    blocks[blocknum].dirty = true;
}

void buffermanager::PinBlock(int blocknum)
{
    blocks[blocknum].pin = true;
}

void buffermanager::UpdateBlock(int blocknum)
{
    blocks[blocknum].Recenttime = time(NULL);
}

int buffermanager::DeleteFile(string filename)
{
    DeleteBlock(filename);
    if(remove(filename.c_str())==0)
        {
            cout<<"Delete success!"<<endl;
            return SUCCESS;
        }
        else
        {
            cout<<"Delete fail!"<<endl;
            return FILE_ERROR;
        }
}

bool buffermanager::FindFile(string filename)
{
    fstream File(filename.c_str());
    if (!File.is_open())
    {
        return false;
    }
    File.close();
    return true;
}

void buffermanager::DeleteBlock(string filename)
{
    for (int i =0; i< BLOCKNUMBER; i++)
        if (blocks[i].filename==filename)
            blocks[i].ClearBlock();
}

int buffermanager::Insert(int blocknum, int offset, char* data)
{
    if (strlen(data)+blocks[blocknum].usedsize>BLOCKSIZE)
    {
        cout<<"TOO LARGE TO FIT IN!"<<endl;
        return BLOCK_INSERTION_FAILURE;
    }

    int i = offset;
    char *ch = data;
    while (*ch)
    {
        blocks[blocknum].content[i++] = *ch;
        ch++;
    }

    DirtBlock(blocknum);
    return SUCCESS;
}

int buffermanager::Delete(int blocknum, int blockoffset, int size)
{
    if (blockoffset+size>BLOCKSIZE)
    {
        cout<<"TOO LARGE TO DELETE"<<endl;
        return BLOCK_DELETION_FAILURE;
    }

    for (int i =blockoffset; i<blockoffset+size;i++)
        if (blocks[blocknum].content[i]!='\n') blocks[blocknum].content[i] = ' ';
    
    //cout<<blocks[blocknum].content<<endl;

    DirtBlock(blocknum);
    return SUCCESS;
}

int buffermanager::GetPosition(int blocknum)
{
    return blocks[blocknum].fileoffset;
}
