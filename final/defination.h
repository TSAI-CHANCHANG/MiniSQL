#ifndef DEFINATION_H
#define DEFINATION_H

const int BLOCKSIZE = 4096;
const int BYTESIZE = 8;
const int BLOCKNUMBER = 10;

#define TYPE_INT 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 3



#define ERROR_TYPE -1
#define ERROR_FILE -2
#define ERROR_INDEX_EXISTS -3
#define ERROR_OFFSET_NOT_EXISTS -4
#define ERROR_UNKNOWN -233

#define NO_SUCH_TABLE 1000
#define NO_SUCH_INDEX 1001
#define HAVE_SUCH_TABLE 1002
#define HAVE_SUCH_INDEX 1003
#define CONSTRAINT_DEFINATION_ERROR 1004
#define WRONG_DEFINATION_FORMAT 1005
#define CONSTRAINT_VIOLATION 1006

#define FILE_ERROR 1050
#define BLOCK_INSERTION_FAILURE 1051
#define BLOCK_DELETION_FAILURE 1052
#define NOT_ENOUGH 1053
#define NO_SUCH_BLOCK 1054

#define SUCCESS 2333

#endif // DEFINATION_H
