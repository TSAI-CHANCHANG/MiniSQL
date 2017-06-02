#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include<string>
#include<time.h>

const int BLOCKSIZE = 4096;
const int BYTESIZE = 8;
const int BLOCKNUMBER = 10;

using namespace std;

class block{
private:
    bool dirty;
    bool pin;
    bool used;
    time_t Recenttime;//最近使用时间

public:
    string filename;//对应文件名
    int fileoffset;//对应在文件中的第几个block
    int usedsize;//已经使用的size
    char content[BLOCKSIZE+1];

    void ClearBlock();
    //将这个块恢复初始化状态

    void SetBlock(string filename, int fileoffset);
    //将这个块根据来源赋值，包括对应文件和文件位置，时间更新等

    int FindUsedSize();
    //更新这个块的大小，返回值为有几个字节被占用了，而不是占用到下标多少的字节

    friend class buffermanager;
};

class buffermanager{
public:
    buffermanager();
    ~buffermanager();

    int FindBlockinBuffer(string fileName, int offset);
    //在buffer中找一个块（没有就从文件中放到buffer里，返回这是第几个buffer block）
    //约定三种文件后缀，table是.tab，index是.idx，catlog是.cat

    char* GetDetail(int blocknum, int blockoffset);
    //给出blocknum和blockoffset，得到对应的指针

    int FindSuitBlockinBuffer(string fileName, int size);
    //给出文件名和需要插入的数据大小，返回一个足够在末端插入这个数据量的块

    void DeleteFile(string filename);
    //drop表或者索引的时候用，将文件删除（同时删除内存中的缓冲区）

    bool FindFile(string filename);
    //查找文件

    void DirtBlock(int blocknum);
    //把一个Block设为dirty

    void PinBlock(int blocknum);
    //将一个block固定住，设为不能修改

    void Insert(int blocknum, char* data);
    //在编号为blocknum的块最后插入data

    void Delete(int blocknum, int blockoffset, int size);
    //从编号为blocknum的块里第offset的位置起删掉大小为size的数据

private:
    void WriteBack(int blocknum);
    //将一个块写回文件中

    void WriteIn(string fileName, int offset, int blocknum);
    //将文件中的一个块读入buffer中的一块

    int GetAnEmptyBlock();
    //找一个空块（没有的话就按照LRU规则删一块）

    void UpdateBlock(int blocknum);
    //更新一个block的使用时间，LRU时候用

    void DeleteBlock(string filename);
};

#endif // BUFFERMANAGER_H
