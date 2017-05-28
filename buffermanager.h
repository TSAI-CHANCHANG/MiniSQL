#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

class position{
public:
    int blocknum;//buffer中的第几个block
    int blockoffset;//这个block中的第几个字节
};

class block{
public:
    bool dirty;
    bool pin;
    const int totalsize;//4096
    int usedsize;//已经使用的size
};

class buffermanager{
public:
    int FindBlockinBuffer(string fileName, int offset);
    //在buffer中找一个块（没有就从文件中放到buffer里，返回这是第几个buffer block）

    position GetInsertPos(int size);
    //找到一个可以插入大小为size个字节的位置（size<4096）

    void DirtBlock(int buffernum);
    //把一个Block设为dirty

    void PinBlock(int buffernum);
    //将一个block固定住，设为不能修改

    void UpdateBlock(int buffernum);
    //更新一个block的使用时间，LRU时候用

private:
    const int blocknumber;
    block blocks[blocknumber];//预计有100个buffer中的block
};

#endif // BUFFERMANAGER_H
