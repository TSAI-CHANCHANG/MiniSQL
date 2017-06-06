# ReadMe - Record Manager

Record Manager主要处理与Record相关的操作，包括数据的插入， 删除与查找。

## Object：RecordManager
用于实例化一个RecordManager类来进行相关的操作。

```cpp
RecordManager recordManagerName;
```

## 成员函数

### insertRecord
向某一表中插入一条数据

如插入的数据不合法， 将会就地报错并结束该函数的执行

#### 原型：

```cpp
bool insertRecord(string tableName, string rawValues);
```

#### 参数：
**tableName: string**  
要插入的表的名字

**rawValues: string**  
要插入的值， 即`insert`语句中的括号里边的部分

#### 返回值：
**bool**  
**true**: 成功插入  
**false**: 插入失败

### deleteRecords
...

### selectRecords
...
