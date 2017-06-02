#include <iostream>
#include <fstream> 
#include<cstdio>
#include<string.h>

#include "buffermanager.h"
#include "runtimeexception.h"

block blocks[BLOCKNUMBER];//预计有10个buffer中的block

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

void buffermanager::WriteBack(int blocknum)
{
    //将一个块写回文件
    if (!blocks[blocknum].used) return;
    const char* filename = blocks[blocknum].filename.c_str();
    fstream File(filename, std::ios::out | std::ios::ate);
    if (!File)
    {
        string ex_info = "file named " + blocks[blocknum].filename + " open failed!";
        throw runtime_error(ex_info);
    }

    int l = File.tellg();
    File.seekg(0, ios::end);
    l = File.tellg()-l;//记录文件长度
    if (blocks[blocknum].fileoffset*BLOCKSIZE>l)
    {
        string ex_info = "file named " + blocks[blocknum].filename + " search failed!";
        throw runtime_error(ex_info);
    }

    File.seekg(blocks[blocknum].fileoffset*BLOCKSIZE,ios::beg);
    for (int i=0;i<BLOCKSIZE;i++)
        File<<blocks[blocknum].content[i];
    File.close();
    //清除这个块的相关信息
    blocks[blocknum].ClearBlock();
}

int buffermanager::FindBlockinBuffer(string fileName, int offset)
{
    //如果这个块已经在buffer中，返回这个块在buffer中的number
    for (int i = 0; i< BLOCKNUMBER; i++)
        if (blocks[i].used&& blocks[i].filename==fileName && blocks[i].fileoffset==offset)
        {
            UpdateBlock(i);
            return i;
        }

    //如果不在，就先找buffer中一个空的块，然后将这个块从文件里写入buffer
    int blocknum = GetAnEmptyBlock();
    WriteIn(fileName,offset,blocknum);
    return blocknum;
}

void buffermanager::WriteIn(string fileName, int offset, int blocknum)
{
    //将这个块的内容从文件中写入
    const char* filename = fileName.c_str();
    fstream File(filename, std::ios::in | std::ios::app);
    if (!File.is_open())
    {
        string ex_info = "file named " + fileName + " open failed!";
        throw runtime_error(ex_info);
    }

    File.seekg(offset*BLOCKSIZE, ios::beg);
    for (int i = 0; i<BLOCKSIZE; i++)
    {
        File.get(blocks[blocknum].content[i]);
        //cout<<blocks[blocknum].content[i];
    }
    File.close();
    //set这个块的相关信息，包括对应文件和对应文件中的offset、最近修改时间等等
    blocks[blocknum].SetBlock(fileName, offset);
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
    if (blocks[LRU].dirty) WriteBack(LRU);
    return LRU;
}

int buffermanager::FindSuitBlockinBuffer(string fileName, int size)
{
    //如果buffer里有可以用的块，直接用
    for (int i = 0; i< BLOCKNUMBER; i++)
        if (blocks[i].used&& blocks[i].filename==fileName && BLOCKSIZE-blocks[i].usedsize>=size)
        {
            UpdateBlock(i);
            return i;
        }

    //没有可以直接用的块，就去文件里一个块一个块地找
    const char* filename = fileName.c_str();
    fstream File(filename);
    if (!File.is_open())
    {
        string ex_info = "file named " + fileName + " open failed!";
        throw runtime_error(ex_info);
    }
    int l = File.tellg();
    File.seekg(0, ios::end);
    l = File.tellg()-l;//记录文件长度
    File.seekg(0, ios::beg);

    char ch;
    long count = 0;
    while (1)
    {
        File >> ch;
        while (ch)
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
    File << 0;
    File.close();
    int blocknum = GetAnEmptyBlock();
    WriteIn(fileName,count/BLOCKSIZE,blocknum);
    return blocknum;
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

void buffermanager::DeleteFile(string filename)
{
    DeleteBlock(filename);
    if(remove(filename.c_str())==0)
        {
            cout<<"Delete success!"<<endl;
        }
        else
        {
            cout<<"Delete fail!"<<endl;
        }
    return ;
}

bool buffermanager::FindFile(string filename)
{
    fstream File(filename);
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

void buffermanager::Insert(int blocknum, char* data)
{
    strcpy(blocks[blocknum].content+blocks[blocknum].usedsize,data);
    blocks[blocknum].usedsize += strlen(data);
}

void buffermanager::Delete(int blocknum, int blockoffset, int size)
{
    char tmp[BLOCKSIZE];
    strcpy(tmp,blocks[blocknum].content);
    int i = blockoffset;
    while (tmp[i+size])
    {
        tmp[i]=tmp[i+size];
        i++;
    }
    while (tmp[i])
        tmp[i++]=0;
    strcpy(blocks[blocknum].content,tmp);
    blocks[blocknum].usedsize = strlen(tmp);
}
