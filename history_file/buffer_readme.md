# Buffer Manager的对外接口

# FindBlockinBuffer

```
int FindBlockinBuffer(string fileName, int offset)
```

## string fileName

*   要获取块的文件的名称

## int offset

*   它是这个文件中的第offset块

*   从0开始

## Returned value

*   一个int是缓冲区中的块数
*   例如 如果你需要一个块，那么我得到它并返回一个BLOCKNUM，你可以读取块[BLOCKNUM] .content，这是一个char *类型的变量

## Description

*   如果块在缓冲区中，我们只返回blocknum
*   如果块不在缓冲区中，我们首先从文件中读取它
*   然而，对于上层，没有什么可担心的，因为当他们达到数据时，它们现在都是缓冲区了

# FindSuitBlockinBuffer

```
void FindSuitBlockinBuffer(string fileName, int size, int* blocknum, int * blockoffset)
```

## string fileName

*   要获取块的文件的名称

## int size

*   要插入的数据的大小
*   例如 你想返回“3 XIAODONG 66666”，大小是16

## int* blocknum

*   返回值
*   它是缓冲区中的块的编号，如上所述

## int* blockoffset

*   返回值
*   content中可以开始的位置
*   注意空格！！！！！
*   例如 如果从块[blocknum] .content [3]开始有8个空格（或换行符），那么我将返回4，因为你必须留下一个空格
*   空白的数量至少比size大2（保证前后有分隔符）
*   但是，如果是空块，则返回0

## Description

*   它是插入操作中有用的功能
*   告诉我你要插入哪个文件的数据多少，我将返回空格和位置
*   我首先找到它是否在缓冲区，如果没有，搜索整个文件。 如果文件为“饱和”，则会在文件尾部附加一个新的块。

# DirtBlock

```
void DirtBlock(int blocknum);
```

## int blocknum

*   给这个块设为dirt

## Description

*   在修改块中的内容后不要忘记调用这个函数！！！！
*   但是，如果您使用buffermanager提供的插入和删除功能，这是不必要的，因为我已经做了这样的事情

# DeleteFile

```
int DeleteFile(string filename);
```

## string filename

*   要删除的文件的名称

## Description

*   如果你不能删除它，它会输出错误

# FindFile

```
bool FindFile(string filename);
```

## string filename

*   要查找的文件的名称

## Returned Value

*   找到它是true，没有找到它是false

# Insert

```
int Insert(int blocknum, int offset, char* data);
```

## int blocknum

*   要插入数据的块

## int offset

*   插入开始的位置（包括它）

## char* data

*   要插入的详细数据

## Description

*   在这个功能中没有检测错误功能！

# Delete

```
int Delete(int blocknum, int blockoffset, int size);
```

## int blocknum

*   要从中删除数据的块

## int blockoffset

*   从哪里（包括这个位置）开始删除

## int size

*   删除数据多长

## Description

*   使用空格填充原始数据

# GetPosition

```
int GetPosition(int blocknum);
```

## int blocknum

*   buffer中的块标号

## returned value

*   这个块是所属文件中的第几块

## Description

*   定位用

# Class Block

```
string filename;
int fileoffset;
int usedsize;
char content[BLOCKSIZE+1];
```

## string filename

*   块中的数据属于哪个文件

## int fileoffset

*   它是文件中的第fileoffset块

## int usedsize

*   在该块中使用了多少字节

## char content[BLOCKSIZE+1]

*   定义BLOCKSIZE = 4096
*   最后一个字符始终为0

## Other

*   block blocks[BLOCKNUMBER];
*   BLOCKNUMBER = 10
