#include "Interpreter.h"
#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>

static char address[1000];
static ifstream ifs;
using namespace std;

/////////////////////////////////////////////////
/////Function No. 1:
/////get the user input 
string getInput()
{
	string SQLSentence;
	string temp;
	char str[100];
	bool finish = false;
	while (!finish)
	{
		cin >> str;
		temp = str;
		SQLSentence = SQLSentence + " " + temp;
		if (SQLSentence[SQLSentence.size() - 1] == ';')
		{
			SQLSentence[SQLSentence.size() - 1] = ' ';
			SQLSentence += ";";
			finish = true;
		}
	}
	return SQLSentence;
}

/////////////////////////////////////////////////
/////Function No. 2:
/////analusis the select clause then change the object of condition class 
int select_clause(string &SQLSentence,  int &SQLCurrentPointer, int &end, condition &SQLCondition)
{
	string currentWord;
	//step1
	SQLCondition.setInstruction(SELECT);

	//step2
	if ((end = SQLSentence.find(" from", SQLCurrentPointer)) == -1)//find " from"
	{
		cout << "Error! Can not find key words \"from\"." << endl;//can not find from
		end = SQLSentence.find(";", SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}

	//step3
	while (SQLSentence[end] == ' ')//select xxx from ooo, this is to find where is the end of xxx
		--end;
	while (SQLSentence[SQLCurrentPointer] == ' ' && end >= SQLCurrentPointer)//select xxx from ooo, this is to find where is the front of xxx
		++SQLCurrentPointer;
	end++;//adjust the pointer to the space right after xxx
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);

	//step4
	if (currentWord.empty() == 1)//can not find col name
	{
		cout << "Error! Can not find col name." << endl;
		end = SQLSentence.find(";", SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}

	//step5
	if (currentWord != "*")//not select all col name
	{
		int f = 0, e = 0;
		string col;
		while (e < currentWord.size())
		{
			if (currentWord[e] == ' ')
			{
				int flag = 0;
				if (currentWord[e - 1] != ',')
					flag = 1;
				while (currentWord[e] == ' ' && e < currentWord.size())
					++e;
				if (flag && currentWord[e] != ',')
				{
					cout << "Error! col name must be separated by \',\'." << endl;
					end = SQLSentence.find(";", SQLCurrentPointer);
					SQLCurrentPointer = end;
					return ERROR;
				}
				else if (currentWord[e] == ',')
					f = ++e;
				else
					f = e;
			}
			while (currentWord[e] != ',' && currentWord[e] != ' ' && e < currentWord.size())
				++e;
			col = currentWord.substr(f, e - f);
			SQLCondition.setColName(col);
			f = e + 1;//move to next col
			if (currentWord[e] == ',')
				++e;
			if (currentWord[e] == ' ')
			{
				int flag = 0;
				if (currentWord[e - 1] != ',')
					flag = 1;
				while (currentWord[e] == ' ' && e <= currentWord.size())
					++e;
				if (e + 1 == currentWord.size() && flag == 1)
				{
					cout << "Error! col name has syntax error." << endl;
					end = SQLSentence.find(";", SQLCurrentPointer);
					SQLCurrentPointer = end;
					return ERROR;
				}
				if (flag && currentWord[e] != ',')
				{
					cout << "Error! col name must be separated by \',\'." << endl;
					end = SQLSentence.find(";", SQLCurrentPointer);
					SQLCurrentPointer = end;
					return ERROR;
				}
				else if (currentWord[e] == ',')
					f = ++e;
				else
					f = e;
			}
		}

	}
	else
	{
		SQLCondition.setColName(currentWord);
	}

	//step6
	while (SQLSentence[end] == ' ')//select xxx from ooo, this is to find where is the front of from
		++end;
	SQLCurrentPointer = end;

	//step7
	end = SQLSentence.find(' ', SQLCurrentPointer);
	if (end == -1)
	{
		cout << "Error! Can not find table name." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}
	SQLCurrentPointer = end;

	//step8
	while (SQLSentence[end] == ' ')//select xxx from ooo, this is to find where is the front of ooo
		++end;
	SQLCurrentPointer = end;
	while (SQLSentence[end] != ';' && SQLSentence[end] != ' ')
		++end;
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);

	//step9
	if (currentWord.empty() || currentWord == "where")
	{
		cout << "Error! the table name can not be found" << endl;
		end = SQLSentence.find(";", SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}

	//step10
	SQLCondition.setTableName(currentWord);
	//step11
	while (SQLSentence[end] == ' ')
		++end;
	SQLCurrentPointer = end;
	if (SQLSentence[end] == ';')
		return SELECT;
	else
	{
		SQLCurrentPointer = SQLSentence.find("where", 0);
		while (SQLSentence[SQLCurrentPointer] != ' ' && SQLSentence[SQLCurrentPointer] != ';')
			++SQLCurrentPointer;
		end = SQLSentence.find(';', 0);
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);
		SQLCondition.setWhereClause(currentWord);
		return SELECT;
	}
}

/////////////////////////////////////////////////
/////Function No. 3:
/////analusis the drop clause then change the object of condition class 
int drop_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition)
{
	string currentWord;

	//step1
	end = SQLSentence.find("table", SQLCurrentPointer);
	if (end != -1)//drop table
	{
		SQLCondition.setInstruction(DROP_TABLE);
		SQLCurrentPointer = end;//move pointer to 't'("drop table oooo")
		while (SQLSentence[end] != ' ' && end < SQLSentence.size())//move across "table"
			++end;
		while (SQLSentence[end] == ' ' && end < SQLSentence.size())// get rid of ' '
			++end;
		if (SQLSentence[end] == ';')//lose table name
		{
			cout << "Error! can not find table name." << endl;
			SQLCurrentPointer = end;
			return ERROR;
		}
		SQLCurrentPointer = end;
		end = SQLSentence.find(';', SQLCurrentPointer);
		if(SQLSentence[end - 1] == ' ')
			--end;
		while (SQLSentence[end] == ' ' && end > SQLCurrentPointer)// get rid of ' '
			--end;
		if (SQLSentence[end] == ';')
			currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);//get table name
		else
			currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);//get table name
		
		//set tableName
		SQLCondition.setTableName(currentWord);
		SQLCurrentPointer = end;
	}
	else//drop index
	{
		end = SQLSentence.find("index", SQLCurrentPointer);
		if (end == -1)
		{
			cout << "Error! Can not find key word 'table' or 'index'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		SQLCondition.setInstruction(DROP_INDEX);
		////
		end = SQLSentence.find(" on", SQLCurrentPointer);
		if (end == -1)
		{
			cout << "Error! Can not find key word /'on'/." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		SQLCurrentPointer++;
		while (SQLSentence[SQLCurrentPointer] != ' ')//across index
			SQLCurrentPointer++;
		while (SQLSentence[SQLCurrentPointer] == ' ')
			SQLCurrentPointer++;
		while (end > SQLCurrentPointer && SQLSentence[end] == ' ')
			--end;
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);

		if (currentWord.empty())
		{
			cout << "Error! Can not find index name." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		SQLCondition.setIndexName(currentWord);
		end = SQLSentence.find("on", SQLCurrentPointer);
		SQLCurrentPointer = end;
		while (SQLSentence[SQLCurrentPointer] != ' ')//across on
			SQLCurrentPointer++;
		while (SQLSentence[SQLCurrentPointer] == ' ')
			SQLCurrentPointer++;
		end = SQLSentence.find(";", SQLCurrentPointer);
		--end;
		while (end > SQLCurrentPointer && SQLSentence[end] == ' ')
			--end;
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
		SQLCondition.setTableName(currentWord);
		SQLCurrentPointer = ++end;
	}
	return DROP;
}

/////////////////////////////////////////////////
/////Function No. 3:
/////analusis the insert clause then change the object of condition class 
int insert_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition)
{
	string currentWord;
	//step0
	SQLCondition.setInstruction(INSERT);

	//step1
	while (SQLSentence[SQLCurrentPointer] == ' ')//move to next word
		++SQLCurrentPointer;
	
	//step2
	if (SQLSentence[SQLCurrentPointer] == ';')
	{
		cout << "Error! Can not find key word 'into'." << endl;
		end = SQLCurrentPointer;
		return ERROR;
	}

	//step3
	end = SQLCurrentPointer;
	while (SQLSentence[end] != ' ' && SQLSentence[end] != ';')
		++end;
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);
	if (currentWord != "into")
	{
		cout << "Error! Can not find key word 'into'." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}
	
	//step4
	SQLCurrentPointer = end;
	end = SQLSentence.find(" values", SQLCurrentPointer);
	if (end == -1)
	{
		cout << "Error! Can not find key word 'values'." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}
	while (SQLSentence[end] == ' ' && end > SQLCurrentPointer)
		--end;
	end += 1;
	while (SQLSentence[SQLCurrentPointer] == ' ' && end > SQLCurrentPointer)
		++SQLCurrentPointer;
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);

	if(currentWord.empty())
	{
		cout << "Error! Can not find table name." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}
	SQLCondition.setTableName(currentWord);

	//step5
	end = SQLSentence.find("values", SQLCurrentPointer);
	while (SQLSentence[end] != ' ' && SQLSentence[end] != ';' && SQLSentence[end] != '(')
		++end;
	if (SQLSentence[end] == ';')
	{
		cout << "Error! Can not find insert values." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}

	//step6
	SQLCurrentPointer = SQLSentence.find('(');
	if (SQLCurrentPointer == -1)
	{
		cout << "Error! Can not find '('." << endl;
		end = SQLSentence.find(';', 0);
		SQLCurrentPointer = end;
		return ERROR;
	}
	end = SQLSentence.find(')');
	if (end == -1)
	{
		cout << "Error! Can not find ')'." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}
	end = SQLSentence.find(';');
	while (SQLSentence[end] != ')')
		--end;
	if (SQLSentence[SQLCurrentPointer + 1] == '(')
	{
		cout << "Error! the InsertValues is empty." << endl;
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;
		return ERROR;
	}
	++SQLCurrentPointer;
	--end;
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
	SQLCondition.setInsertValues(currentWord);
	//step7,end
	end = SQLSentence.find(';', SQLCurrentPointer);
	SQLCurrentPointer = end;
	return INSERT;
}

/////////////////////////////////////////////////
/////Function No. 4:
/////analusis the create clause then change the object of condition class
int create_clause(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition)
{
	string currentWord;

	//step1
	end = SQLSentence.find("table", SQLCurrentPointer);

	if (end != -1) //"table" exists
	{
		SQLCondition.setInstruction(CREATE_TABLE);
		//step2.a
		SQLCurrentPointer = end; // adjust the pointer to 't' in table
		while (SQLSentence[SQLCurrentPointer] != ' ' && SQLSentence[SQLCurrentPointer] != ';' && SQLCurrentPointer <= SQLSentence.size())
			++SQLCurrentPointer;//across "table"
		end = SQLSentence.find('(', 0);
		if (end == -1)
		{
			cout << "Error! Can not find key symbol '('." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step3.a
		--end;//adjust to the space right before the '('
		while (SQLSentence[end] == ' ' && SQLCurrentPointer < end)
			--end;
		if (SQLCurrentPointer == end)
		{
			cout << "Error! Can not find table name." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step4.a
		while (SQLSentence[SQLCurrentPointer] == ' ' && SQLCurrentPointer < end)
			++SQLCurrentPointer;
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
		int i = 0;
		while (currentWord[i] != ' ' && i < currentWord.size())
			++i;
		if (i != currentWord.size())
		{
			cout << "Error! find a sytnax fault between \"table\" and '('." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		SQLCondition.setTableName(currentWord);

		//step5.a
		SQLCurrentPointer = SQLSentence.find('(', 0);
		end = SQLSentence.find(';', SQLCurrentPointer);
		while (SQLSentence[end] != ')')
			--end;
		if (end == -1)
		{
			cout << "Error! Can not find key symbol ')'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step6.a
		if (end == SQLCurrentPointer + 1)
		{
			cout << "Error! Can not find attribute." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		
		//step7.a
		++SQLCurrentPointer;
		--end;
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
		SQLCondition.setAttribute(currentWord);
		return CREATE_TABLE;
	}
	else//create "index"?
	{
		//step2.b
		end = SQLSentence.find("index", 0);
		if (end == -1)
		{
			cout << "Error! Can not find key word 'table' or 'index'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		SQLCondition.setInstruction(CREATE_INDEX);
		//step3.b
		end = SQLSentence.find("on", 0);
		if (end == -1)
		{
			cout << "Error! Can not find key word 'on'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step4.b
		SQLCurrentPointer = SQLSentence.find("index", 0);
		while (SQLSentence[SQLCurrentPointer] != ' ' && SQLSentence[SQLCurrentPointer] != ';' && SQLCurrentPointer < SQLSentence.size())
			++SQLCurrentPointer;//across index
		
		//step5.b
		--end;
		while (SQLSentence[end] == ' ' && SQLCurrentPointer < end)
			--end;
		while (SQLSentence[SQLCurrentPointer] == ' ' && SQLCurrentPointer < end)
			++SQLCurrentPointer;
		if (end == SQLCurrentPointer)
		{
			cout << "Error! Can not find index name." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step6.b
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
		int i = 0;
		while (currentWord[i] != ' ' && i < currentWord.size())
			++i;
		if (i != currentWord.size())
		{
			cout << "Error! find a sytnax fault between \'index\" and \"on\"." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		SQLCondition.setIndexName(currentWord);

		//step7.b
		end = SQLSentence.find("on", 0);
		if (end == -1)
		{
			cout << "Error! Can not find key word 'on'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step8.b
		SQLCurrentPointer = end;
		while (SQLSentence[SQLCurrentPointer] == 'o' || SQLSentence[SQLCurrentPointer] == 'n')
			++SQLCurrentPointer;//across 'on'

		//step9.b
		end = SQLSentence.find('(', 0);
		if (end == -1)
		{
			cout << "Error! Can not find key symbol '('." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step10.b
		--end;
		while (SQLSentence[end] == ' ' && SQLCurrentPointer < end)
			--end;
		while (SQLSentence[SQLCurrentPointer] == ' ' && SQLCurrentPointer < end)
			++SQLCurrentPointer;
		if (end == SQLCurrentPointer)
		{
			cout << "Error! Can not find table name." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step11.b
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
		i = 0;
		while (currentWord[i] != ' ' && i < currentWord.size())
			++i;
		if (i != currentWord.size())
		{
			cout << "Error! find a sytnax fault between \"on\" and '('." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		SQLCondition.setTableName(currentWord);

		//step12.b
		SQLCurrentPointer = SQLSentence.find('(', 0);
		end = SQLSentence.find(')', SQLCurrentPointer);
		if (end == -1)
		{
			cout << "Error! Can not find key symbol '）'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step14.b
		if (end == SQLCurrentPointer + 1)
		{
			cout << "Error! Can not find attribute." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}

		//step7.a
		++SQLCurrentPointer;
		--end;
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
		SQLCondition.setAttribute(currentWord);
		return CREATE_INDEX;
	}
}

/////////////////////////////////////////////////
/////Function No. 5:
/////analusis the delect clause then change the object of condition class
int delect_clauese(string &SQLSentence, int &SQLCurrentPointer, int &end, condition &SQLCondition)
{
	string currentWord;
	SQLCondition.setInstruction(DELETE);
	//step1
	if (SQLSentence.find("delete from", 0) == -1)//error
	{
		if (SQLSentence.find("from", 0) == -1)//can not find from
		{
			cout << "Error! Can not find key word 'from'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		else
		{
			cout << "Error! There are some syntax error between \"delete\" and \"from\"." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
	}
	
	//step2
	SQLCurrentPointer = SQLSentence.find("from", 0);

	//step3
	while (SQLSentence[SQLCurrentPointer] != ' ' && SQLSentence[SQLCurrentPointer] != ';')
		++SQLCurrentPointer;//now pointer is point to "from "'s end space

	//step4
	end = SQLSentence.find("where", 0);

	if (end == -1)
	{
		end = SQLSentence.find(';', 0);
		--end;
		while (SQLSentence[end] == ' ' && end > SQLCurrentPointer)
			--end;
		while (SQLSentence[SQLCurrentPointer] == ' ' && end > SQLCurrentPointer)
			++SQLCurrentPointer;
		if (SQLCurrentPointer == end)
		{
			cout << "Error! Can not find table name." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
		int i = 0;
		while (currentWord[i] != ' ' && i < currentWord.size())
			++i;
		if (i != currentWord.size())
		{
			cout << "Error! find a sytnax fault between \"from\" and ';'." << endl;
			end = SQLSentence.find(';', SQLCurrentPointer);
			SQLCurrentPointer = end;
			return ERROR;
		}
		SQLCondition.setTableName(currentWord);
		SQLCondition.setWhereClause("");
		end = SQLSentence.find(';', SQLCurrentPointer);
		SQLCurrentPointer = end;//end
		return DELETE;
	}
	
	--end;
	while (SQLSentence[end] == ' ' && end > SQLCurrentPointer)
		--end;
	while (SQLSentence[SQLCurrentPointer] == ' ' && end > SQLCurrentPointer)
		++SQLCurrentPointer;
	if (SQLCurrentPointer == end)
	{
		cout << "Error! Can not find table name." << endl;
		end = SQLSentence.find(';', 0);
		SQLCurrentPointer = end;
		return ERROR;
	}
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer + 1);
	int i = 0;
	while (currentWord[i] != ' ' && i < currentWord.size())
		++i;
	if (i != currentWord.size())
	{
		cout << "Error! find a sytnax fault between \"from\" and 'where'." << endl;
		end = SQLSentence.find(';', 0);
		SQLCurrentPointer = end;
		return ERROR;
	}
	SQLCondition.setTableName(currentWord);

	//find where clause
	SQLCurrentPointer = SQLSentence.find("where", 0);
	while (SQLSentence[SQLCurrentPointer] != ' ' && SQLSentence[SQLCurrentPointer] != ';')
		++SQLCurrentPointer;
	end = SQLSentence.find(';', 0);
	currentWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);
	SQLCondition.setWhereClause(currentWord);


	//end
	end = SQLSentence.find(';', SQLCurrentPointer);
	SQLCurrentPointer = end;
	return SELECT;
}

/////////////////////////////////////////////////
/////Function No. 6:
/////
int execute_clause(string &fileName, condition &SQLCondition, BPLUSTREE &BTree, buffermanager &bufManager)
{
	std::ifstream file((fileName).c_str());
	if (!file)
		std::cout << "Error! File does not exist" << std::endl;
	else
	{
		std::string buf;
		while (getline(file, buf))
		{
			while (buf.empty())
			{
				getline(file, buf);
				if (file.eof())
				{
					return QUIT_NUMBER;
				}
			}
			int len = buf.size();
			while (len && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
			{
				--len;
			}
			interpreter(buf, SQLCondition, BTree, bufManager);
			SQLCondition.clearClass();
		}
	}
	return EXEC;
}

extern block blocks[BLOCKNUMBER];

void API(condition &SQLCondition, BPLUSTREE &BTree, buffermanager &bufManager)
{

	catalogmanager catalogManager(bufManager);
	RecordManager recordManager(SQLCondition.showTableName(), catalogManager, bufManager, BTree);
	int instruction;
	instruction = SQLCondition.showInstruction();
	switch (instruction)
	{
	case INSERT: {
		recordManager.insertRecord(SQLCondition.showInsertValues());
		break;
	}
	case SELECT: {
		vector<string> attrs = SQLCondition.showColName();
		recordManager.selectRecords(attrs, SQLCondition.showWhereClause());
		break;
	}
	case DELETE: {
		recordManager.deleteRecords(SQLCondition.showWhereClause());
		break;
	}
	case CREATE_TABLE: {
		catalogManager.CreateTable(SQLCondition.showTableName(), SQLCondition.showAttribute(), BTree);
		break;
	}
	case CREATE_INDEX: {
		catalogManager.CreateIndex(SQLCondition.showIndexName(), SQLCondition.showTableName(), SQLCondition.showAttribute(), BTree);
		break;
	}
	case DROP_TABLE: {
		catalogManager.DropTable(SQLCondition.showTableName(), BTree);
		break;
	}
	case DROP_INDEX: {
		catalogManager.DropIndex(SQLCondition.showIndexName(), SQLCondition.showTableName(), BTree);
		break;
	}
	default:
		break;
	}
}

int interpreter(string &SQLSentence, condition &SQLCondition, BPLUSTREE &BTree, buffermanager &bufManager)
{
	string firstWord;
	int SQLCurrentPointer = 0, end = 0;
	int code = 0;


	//////////////////////input part/////////////////////////////////////////////
	if(SQLSentence.empty())
		SQLSentence = getInput();
	//////////////////////instruction analysis part//////////////////////////////////


	while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
		SQLCurrentPointer++;
	end = SQLSentence.find(' ', SQLCurrentPointer);
	if (end == -1)
	{
		end = SQLSentence.find(';', SQLCurrentPointer);//example:"quit;", then there does not have a ' '.
	}
	firstWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);//get the first word from SQL sentence
	SQLCurrentPointer = end;//pointer move forward



	///////start to analysis the sentence
	if (firstWord == "select")
	{
		code = select_clause(SQLSentence, SQLCurrentPointer, end, SQLCondition);
	}
	else if (firstWord == "insert")
	{
		code = insert_clause(SQLSentence, SQLCurrentPointer, end, SQLCondition);
	}
	else if (firstWord == "quit")
	{
		string nextWord;
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' after the first word
			SQLCurrentPointer++;
		end = SQLSentence.find(' ', SQLCurrentPointer);//get the next word
		nextWord = SQLSentence.substr(SQLCurrentPointer, end - SQLCurrentPointer);//check whether the quit instruction
																				  //has other words. example:"quit select;"
		if (nextWord == ";")//we can only find ';'	
			return QUIT_NUMBER;
		else
			cout << "Error! Quit instruction should has only one key word \"quit\"\n";
	}
	else if (firstWord == "delete")
	{
		code = delect_clauese(SQLSentence, SQLCurrentPointer, end, SQLCondition);
	}
	else if (firstWord == "create")
	{
		code = create_clause(SQLSentence, SQLCurrentPointer, end, SQLCondition);
	}
	else if (firstWord == "exec")//execute the script
	{
		string temp;
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' from the beginning of the sentence
			SQLCurrentPointer++;
		end = SQLSentence.find(' ', SQLCurrentPointer);
		temp = SQLSentence.substr(SQLCurrentPointer + 1, end - SQLCurrentPointer - 2);
		strcpy(address, temp.c_str());//copy the address to array
		execute_clause(temp, SQLCondition, BTree, bufManager);


		SQLCurrentPointer = end;
	}
	else if (firstWord == "drop")
	{
		code = drop_clause(SQLSentence, SQLCurrentPointer, end, SQLCondition);
	}
	else//input is wrong
	{
		cout << "Error! Doesn't found a instruction key word!" << endl;
		end = SQLSentence.find(';', 0);
		SQLSentence.erase(0, end + 1);
		return ERROR;
	}
	if (SQLSentence[SQLCurrentPointer + 1] == ';')
		SQLCurrentPointer += 2;
	else
	{
		while (SQLSentence[SQLCurrentPointer] == ' ')//get rid of the ' ' in the rest fo the sentence
			SQLCurrentPointer++;
		while (SQLSentence[SQLCurrentPointer] == ';')//find ';'
			SQLCurrentPointer++;

	}
	end = SQLSentence.find(';', 0);
	SQLSentence.erase(0, end + 1);//clear
	API(SQLCondition, BTree, bufManager);
	return code;
}

