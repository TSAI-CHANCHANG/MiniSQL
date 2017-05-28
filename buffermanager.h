#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include<time.h>
const int BLOCKSIZE = 4096;
const int BYTESIZE = 8;

using namespace std;

class position{
public:
    int blocknum;//buffer中的第几个block
    int blockoffset;//这个block中的第几个字节
};

class block{
private:
    string filename;//对应文件名
    int fileoffset;//对应在文件中的第几个block
    bool dirty;
    bool pin;
    bool used;
    int usedsize;//已经使用的size
    char content[BLOCKSIZE];
    time_t Recenttime;//最近使用时间

    void ClearBlock(){
        for (int i;i<BLOCKSIZE;i++) content[i]=0;
        filename = "";
        dirty = pin = used = false;
        usedsize = 0;
        fileoffset = 0;
    }

    void SetBlock(string filename, int fileoffset)
    {
        this->filename = filename;
        this->fileoffset = fileoffset;
        dirty = false;
        pin = false;
        used = true;
        usedsize = this->FindUsedSize;
        Recenttime = time(NULL);
    }

    int FindUsedSize()
    {
        for (int i = 0; i< BLOCKSIZE; i++)
            if (content[i]=='\0') return i-1;
    }

    friend class buffermanager;
};

class buffermanager{
public:
    buffermanager();
    ~buffermanager();

    int FindBlockinBuffer(string fileName, int offset);
    //在buffer中找一个块（没有就从文件中放到buffer里，返回这是第几个buffer block）
    //约定三种文件后缀，table是.tab，index是.idx，catlog是.cat

    position GetInsertPos(string fileName,int size);
    //找到一个可以插入大小为size个字节的位置（size<4096）

private:
    void WriteBack(int blocknum);
    //将一个块写回文件中

    void WriteIn(string fileName, int offset, int blocknum);
    //将文件中的一个块读入buffer中的一块

    int GetAnEmptyBlock();
    //找一个空块（没有的话就按照LRU规则删一块）

    void DirtBlock(int blocknum);
    //把一个Block设为dirty

    void PinBlock(int blocknum);
    //将一个block固定住，设为不能修改

    void UpdateBlock(int blocknum);
    //更新一个block的使用时间，LRU时候用
};

const int BLOCKNUMBER = 10;
block blocks[BLOCKNUMBER];//预计有10个buffer中的block

#endif // BUFFERMANAGER_H
