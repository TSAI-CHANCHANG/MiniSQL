# .tab�ļ��ĸ�ʽ

```
tablename blocknum attrnum primary Attrname1 Attrtype1 attrlength1 primary1(bool) indexname1(string) unique1(bool)  Attrname2 Attrtype2����    
```

## tablename

*   ����

## blocknum

*   RECORD�ڡ�.rec���ļ�����Ҫ���ٿ�

## attrnum

*   �����ж�������

## primary

*   ������λ�ã���0��ʼ��-1Ϊ������

## Attrname1

*   ��һ�����Ե�����

## attrlength1

*   ��һ�����Եĳ��ȣ�int��floatΪ0��char��6��Ϊ6

## primary1

*   �Ƿ�������

## indexname1

*   �����ϵ�indexname��û�������ľ��ǡ�noindex��

## unique1

*   �Ƿ��Ƕ�һ�޶���

# Class Table

## int blocknum

*   RECORD���ж��ٿ���Ҫ�ڡ�.rec���ļ���

## short primary

*   ������λ�ô�0��ʼ��-1Ϊ������

## int Attrnum

*   �����ж�������

## vector <attribute>Attr</attribute>

*   ���Ե�����

## string getName() const

## void setName(string tbname)

# Class Attribute

## short type

```
#define TYPE_INT 1
#define TYPE_FLOAT 2
#define TYPE_CHAR 4
```

## short length

*   ֻ�е�type��TYPE_CHARʱ�����������壬����char�����Ƕ೤

## string attrname

*   ���Ե�����

## bool primary

*   �Ƿ�������

## bool unique

*   �Ƿ�Ψһ

## string indexname

*   �����Ե��������ƣ�������Ϊ��noindex��

# GetTable

```
Table GetTable(string Tablename);
```

## string Tablename

*   ������ƣ��ļ�����tablename +'.tab'

## Return Value

*   �����һ������

## Description

*   �����йر�����ϸ��Ϣ

# CreateTable

```
int CreateTable(string Tablename, string Attributes,BPLUSTREE& BT);
```

## string Tablename

*   ��API�ṩ�ı������

## string Attributes

*   ����֮����ַ���

## BPLUSTREE& BT

*   ������������B +������API�ṩ��API����Ӧ����һ��BPLUSTREE�����

## Returned Value

```
#define HAVE_SUCH_TABLE 1002
#define CONSTRAINT_DEFINATION_ERROR 1004
#define WRONG_DEFINATION_FORMAT 1005
#define CONSTRAINT_VIOLATION 1006
#define SUCCESS 2333
```

## Description

*   �յ�ϸ�ڲ�������
*   �����ԣ��ַ�������','����
*   ��Name char��10��unique��һ�������͵�defAtt��string attDef����һ������
*   �������һ����primary key��ID����һ�����ַ���
*   ���Զ���һ���հ�
*   ���Լ�⼸�����д���ĸ�ʽ������д��󣬷���ֵ����SUCCESS
*   ��������������һ���ļ����������е�ϸ�ڣ�ʹ��SetTable��Table tab��
*   �������һ���������������������������������ƣ�������һ���յ��ļ�'indexname.idx'��'tablename.rec'������һ���յ�B����

# DropTable

```
int DropTable(string Tablename, BPLUSTREE& BT)
```

## string Tablename

*   ����

## BPLUSTREE& BT

*   ͬ��

## return value

```
#define NO_SUCH_TABLE 1000
#SUCCESS 2333
```

## Description

*   �����ҵ������Ƿ����κ�������������У�Ȼ��ʹ��BPLUSTREEɾ������
*   Ȼ��ɾ��'.rec'��'.tab'�ļ�

# CreateIndex

```
int CreateIndex(string Indexname, string Tablename, string Attrname, BPLUSTREE& BT)
```

## string Indexname

*   ������

## string Tablename

*   ����

## string Attrname

*   ���ϵ�������

## BPLUSTREE& BT

*   ͬ��

## Returned Value

```
#define NO_SUCH_TABLE 1000
#define HAVE_SUCH_INDEX 1003
#define CONSTRAINT_VIOLATION 1006
#define SUCCESS 2333
```

## Description

*   ���ȣ��ҵ��������
*   ��Σ�����tab.Attr [i] .indexname = indexname;
*   Ȼ��׼������BPLUSS�����������¹���

    > ��'.rec'��ȡ��¼����buffer manager��
    > 
    > ��ȡ������Ҫ�ļ�¼��BPLUS���еĹؼ��֣�
    > 
    > ��key block offsetд��'.idx'�ļ�
    > 
*   ʹ��'.idx'�ļ�����BPLUSTREE CreateIndex

# DropIndex

```
int DropIndex(string Indexname, string Tablename, BPLUSTREE& BT)
```

## string Indexname

*   ������

## string Tablename

*   �������ڵı�

## BPLUSTREE& BT

*   ͬ��

## Returned Value

```
#define NO_SUCH_TABLE 1000
#define NO_SUCH_INDEX 1001
#SUCCESS 2333
```

## Description

*   ɾ��һ���������
*   ���Է���һЩ�������� û������������
*   ��B Plus Tree�е���DropIndex��ɾ����.idx������BPLUSTREE��ɣ�
*   ��tab.attr [i] .indexnameת��Ϊ��noindex��