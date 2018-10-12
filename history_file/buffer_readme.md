# Buffer Manager�Ķ���ӿ�

# FindBlockinBuffer

```
int FindBlockinBuffer(string fileName, int offset)
```

## string fileName

*   Ҫ��ȡ����ļ�������

## int offset

*   ��������ļ��еĵ�offset��

*   ��0��ʼ

## Returned value

*   һ��int�ǻ������еĿ���
*   ���� �������Ҫһ���飬��ô�ҵõ���������һ��BLOCKNUM������Զ�ȡ��[BLOCKNUM] .content������һ��char *���͵ı���

## Description

*   ������ڻ������У�����ֻ����blocknum
*   ����鲻�ڻ������У��������ȴ��ļ��ж�ȡ��
*   Ȼ���������ϲ㣬û��ʲô�ɵ��ĵģ���Ϊ�����Ǵﵽ����ʱ���������ڶ��ǻ�������

# FindSuitBlockinBuffer

```
void FindSuitBlockinBuffer(string fileName, int size, int* blocknum, int * blockoffset)
```

## string fileName

*   Ҫ��ȡ����ļ�������

## int size

*   Ҫ��������ݵĴ�С
*   ���� ���뷵�ء�3 XIAODONG 66666������С��16

## int* blocknum

*   ����ֵ
*   ���ǻ������еĿ�ı�ţ���������

## int* blockoffset

*   ����ֵ
*   content�п��Կ�ʼ��λ��
*   ע��ո񣡣�������
*   ���� ����ӿ�[blocknum] .content [3]��ʼ��8���ո񣨻��з�������ô�ҽ�����4����Ϊ���������һ���ո�
*   �հ׵��������ٱ�size��2����֤ǰ���зָ�����
*   ���ǣ�����ǿտ飬�򷵻�0

## Description

*   ���ǲ�����������õĹ���
*   ��������Ҫ�����ĸ��ļ������ݶ��٣��ҽ����ؿո��λ��
*   �������ҵ����Ƿ��ڻ����������û�У����������ļ��� ����ļ�Ϊ�����͡���������ļ�β������һ���µĿ顣

# DirtBlock

```
void DirtBlock(int blocknum);
```

## int blocknum

*   ���������Ϊdirt

## Description

*   ���޸Ŀ��е����ݺ�Ҫ���ǵ������������������
*   ���ǣ������ʹ��buffermanager�ṩ�Ĳ����ɾ�����ܣ����ǲ���Ҫ�ģ���Ϊ���Ѿ���������������

# DeleteFile

```
int DeleteFile(string filename);
```

## string filename

*   Ҫɾ�����ļ�������

## Description

*   ����㲻��ɾ�����������������

# FindFile

```
bool FindFile(string filename);
```

## string filename

*   Ҫ���ҵ��ļ�������

## Returned Value

*   �ҵ�����true��û���ҵ�����false

# Insert

```
int Insert(int blocknum, int offset, char* data);
```

## int blocknum

*   Ҫ�������ݵĿ�

## int offset

*   ���뿪ʼ��λ�ã���������

## char* data

*   Ҫ�������ϸ����

## Description

*   �����������û�м������ܣ�

# Delete

```
int Delete(int blocknum, int blockoffset, int size);
```

## int blocknum

*   Ҫ����ɾ�����ݵĿ�

## int blockoffset

*   ������������λ�ã���ʼɾ��

## int size

*   ɾ�����ݶ೤

## Description

*   ʹ�ÿո����ԭʼ����

# GetPosition

```
int GetPosition(int blocknum);
```

## int blocknum

*   buffer�еĿ���

## returned value

*   ������������ļ��еĵڼ���

## Description

*   ��λ��

# Class Block

```
string filename;
int fileoffset;
int usedsize;
char content[BLOCKSIZE+1];
```

## string filename

*   ���е����������ĸ��ļ�

## int fileoffset

*   �����ļ��еĵ�fileoffset��

## int usedsize

*   �ڸÿ���ʹ���˶����ֽ�

## char content[BLOCKSIZE+1]

*   ����BLOCKSIZE = 4096
*   ���һ���ַ�ʼ��Ϊ0

## Other

*   block blocks[BLOCKNUMBER];
*   BLOCKNUMBER = 10
