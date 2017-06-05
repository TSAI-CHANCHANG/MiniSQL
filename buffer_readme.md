# Public Functions in Buffer Manager

# FindBlockinBuffer

```
int FindBlockinBuffer(string fileName, int offset)
```

## string fileName

*   The name of the file you want to get the block

## int offset

*   It is the No.offset block in this file.

*   The No. is beginning with 0

## Returned value

*   An int which is the number of the blocks in the buffer
*   e.g. If you need a block, then I get it and return an BLOCKNUM,you can read it in blocks[BLOCKNUM].content, which is a char* type variable

## Description

*   If the block is in buffer, we just return the blocknum
*   If the block is not in buffer, we first read it from the file
*   However, for the higher level, there is nothing to worry about it because when they get reach to data, they are all int buffer now

# FindSuitBlockinBuffer

```
void FindSuitBlockinBuffer(string fileName, int size, int* blocknum, int * blockoffset)
```

## string fileName

*   The name of the file you want to get the block

## int size

*   The size of the data you want to insert in
*   e.g. you want to return "3 XIAODONG 66666", the size is 16

## int* blocknum

*   The return value
*   It is the number of the block in buffer, like the one above

## int* blockoffset

*   The return value
*   It is the position the content can begin with
*   WATCH FOR THE BLANKS!
*   e.g. if there are 8 spaces which begin at blocks[blocknum].content[3], then I will return 4 because you have to leave a space for interva
*   The number of the blanks is at leas 2 more than size
*   e.g. However, if it is an empty block, 0 will be returned

## Description

*   It is a function useful in insert operation
*   Tell me in which file you want to insert data of how much, and I will return the space and the position
*   I first fint whether it is in buffer, if not, search in the entire file. If the file is 'full', a new block will be append at the tail of the file.

# DirtBlock

```
void DirtBlock(int blocknum);
```

## int blocknum

*   The number of the block to set dirt

## Description

*   DO NOT FORGET IT AFTER REVISING THE CONTENT IN THE BLOCKS!
*   However, if you use the Insert and Delete function provided by buffermanager, this is needless because I have done such thing

# DeleteFile

```
int DeleteFile(string filename);
```

## string filename

*   The name of the file to delete

## Description

*   If you cannot delete it, it will output error

# FindFile

```
bool FindFile(string filename);
```

## string filename

*   The name of the file to find

## Returned Value

*   true for finding it, false for not finding it

# Insert

```
int Insert(int blocknum, int offset, char* data);
```

## int blocknum

*   The block you want to insert data in

## int offset

*   The position from which the insert begin(including it)

## char* data

*   The detailed data you want to insert in

## Description

*   There is no detect of corrction in this function!

# Delete

```
int Delete(int blocknum, int blockoffset, int size);
```

## int blocknum

*   The block you want to delete data from

## int blockoffset

*   From where(including) to delete

## int size

*   Delete how long the data

## Description

*   Using blanks to stuff the origin data

# Class Block

```
string filename;
int fileoffset;
int usedsize;
char content[BLOCKSIZE+1];
```

## string filename

*   the data in the block is belong to which file

## int fileoffset

*   it is the No.fileoffset block in the file

## int usedsize

*   howmuch bytes are used in this block

## char content[BLOCKSIZE+1]

*   define BLOCKSIZE = 4096
*   the last char is always 0

## Other

*   block blocks[BLOCKNUMBER];
*   BLOCKNUMBER = 10
