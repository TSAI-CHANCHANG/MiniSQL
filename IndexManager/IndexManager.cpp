#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<stdlib.h>
#include"IndexManager.h"

using namespace std;

BPLUSTREE::BPLUSTREE(int block_size):block_size(block_size)
{
	ifstream file("allindex.txt");
	string file_name, attribute;
	int type;
	while (1)
	{
		if (file.eof())
			break;
		file >> attribute;
		file >> file_name;
		index_to_file[attribute] = file_name;
		file >> type;
		//cout<<file_name<<" "<<type;//............................
		if (type == TYPE_INT)
		{
			//Create int B+Tree and add root node pointer into map
			CreateTree(type, &file_name);
		}
		else if (type == TYPE_FLOAT)
		{
			//Create float B+Tree and add root node pointer into map
		}
		else if (type == TYPE_CHAR)
		{
			//Create char B+Tree and add root node pointer into map
		};
	}
	file.close();
}

BPLUSTREE::~BPLUSTREE()
{
}

int BPLUSTREE::CreateTree(int type, string *file_name)
{
	ifstream index(*file_name);

	if (!index)
		return ERROR_FILE;
	if (type == TYPE_INT)
	{
		INTNODE *root= new INTNODE();
		root->father = NULL;
		//Create an empty root node and add it into map
		int_map[*file_name] = root;
		//cout << "Create a node";//......................................
		string key;
		int turple;
		while (1)
		{
			if (index.eof())
				break;
			index >> key >> turple;
			AddNode(type, file_name, &key, turple);
		}
	}
	else if (type == TYPE_FLOAT)
	{
		
	}
	else if (type == TYPE_CHAR)
	{
		
	}	
	else
	{
		index.close();
		return ERROR_UNKNOWN;
	}
	index.close();
	return 0;
}

int BPLUSTREE::CreateIndex(string * attribute, string * file_name, int type)
{
	//If there exits index on this attribute or with the same index_name
	ifstream check("allindex.txt");
	string s1, s2;
	int temp;

	while (1)
	{
		check >> s1;
		if (check.eof())
			break;
		check >> s2 >> temp;
		if (*attribute != s1 && *file_name != s2)
			continue;
		check.close();
		//cout << "Index exits" << endl;//..................................
		return ERROR_INDEX_EXISTS;
	}
	check.close();
	//Add this index into manager file 
	ofstream allindex("allindex.txt", ios::app);
	
	allindex << *attribute << " " << *file_name << " " << type << endl;
	allindex.close();
	//Create B+Tree
	CreateTree(type, file_name);

	return 0;
}

int BPLUSTREE::DropIndex(string * file_name)
{
	string cmd, attribute, filen;
	
	cmd = "DEL " + *file_name;
	system(cmd.c_str());
	cmd = "rename allindex.txt all.txt";
	system(cmd.c_str());
	
	ifstream old("all.txt");
	ofstream file("allindex.txt");
	int type;
	while (1)
	{
		if (old.eof())
			break;
		old >> attribute;
		old >> filen;
		old >> type;
		if (filen == *file_name) continue;
		file << attribute << " " << filen << " " << type << endl;
	}
	old.close();
	file.close();
	cmd = "DEL all.txt";
	system(cmd.c_str());
	return 0;
}

int BPLUSTREE::AddNode(int type, string * file_name, string *skey, int turple)
{
	int degree = 4;//GetDegree(block_size, type);
	if (type == TYPE_INT)
	{
		INTNODE *root = int_map[*file_name];
		INTNODE *n = new INTNODE(), *p;
		int key;
		stringstream ss(*skey);

		p = root;
		ss >> key;
		//cout << degree << endl;//...........................................
		//The first node
		if (root->key.size() == 0)
		{
			root->key.push_back(key);
			root->turple.push_back(turple);
			return SUCCESS;
		}
		//Add node to a non-empty tree
		while (p->turple.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= key)
				i++;
			i++;
			if (p->key[0] > key) i = 0;
			p = p->pointer[i];
		}
		int i = 0;
		while (i + 1 < p->key.size() &&  p->key[i] <= key) i++;
		if (p->key[i] > key)
			p->key.insert(p->key.begin() + i, key);
		else p->key.push_back(key);
		p->turple.insert(p->turple.begin() + i, turple);
		//Split from leaf
		while (p->key.size() == degree)
		{
			//Split root
			if (p == root)
			{
				INTNODE *t1, *t2;
		
				t2 = new INTNODE;
				t1 = new INTNODE;
				//Only root
				if (p->turple.size() != 0)
				{
					//Copy left half to t1
					for (int i = 0; i < (degree + 1)/ 2; i++)
					{
						t1->key.push_back(p->key[i]);
						if (p->turple.size() != 0)
							t1->turple.push_back(p->turple[i]);
					}
					//Copy right half  to t2	
					for (int i = (degree + 1) / 2; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						if (p->turple.size() != 0)
							t2->turple.push_back(p->turple[i]);
					}
					//Link t1 and t2 as they are leaves
					t1->pointer.push_back(t2);
					p->key.clear();
					p->key.push_back(t2->key[0]);
				}
				else
				{
					//Copy left half to t1
					for (int i = 0; i < degree / 2; i++)
					{
						t1->key.push_back(p->key[i]);
						t1->pointer.push_back(p->pointer[i]);
					}
					t1->pointer.push_back(p->pointer[degree / 2]);
					//Copy right half  to t2
					for (int i = degree / 2 + 1; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						t2->pointer.push_back(p->pointer[i]);
					}
					t2->pointer.push_back(p->pointer[degree]);
					//Add one key to root
					int temp = p->key[degree / 2];
					p->key.clear();
					p->key.push_back(temp);
				}				
				t1->father = t2->father = p;
				//Clear root
				p->turple.clear();
				p->pointer.clear();
				p->pointer.push_back(t1);
				p->pointer.push_back(t2);		
				break;
			}
			else
			{
				INTNODE *t;

				//Split leaf node
				if (p->turple.size() != 0)
				{
					t = new INTNODE;
					t->father = p->father;
					if (p->pointer.size() != 0)
						t->pointer.push_back(p->pointer[0]);
					p->pointer.clear();
					p->pointer.push_back(t);
					//Cut [m/2] to m-1 to t
					int temp = (degree + 1) / 2;
					while (p->key.size() > temp)
					{
						t->key.push_back(p->key[temp]);
						t->turple.push_back(p->turple[temp]);
						p->key.erase(p->key.begin() + temp);
						p->turple.erase(p->turple.begin()+temp);
					}
					//Add t to their father's pointer list
					temp = 0;
					while (p->father->pointer[temp] != p)
						temp++;
					p = p->father;
					temp++;
					p->pointer.insert(p->pointer.begin() + temp, t);
					if (temp != 1)
						temp--;
					p->key.insert(p->key.begin() + temp, t->key[0]);
				}
				//Split non-leaf node
				else
				{
					t = new INTNODE;
					t->father = p->father;
					int temp = degree / 2 + 1;
					while (p->key.size() > temp)
					{
						t->key.push_back(p->key[temp]);
						t->pointer.push_back(p->pointer[temp]);
						p->key.erase(p->key.begin() + temp);
						p->pointer.erase(p->pointer.begin()+temp);
					}
					t->pointer.push_back(p->pointer[temp]);
					p->pointer.erase(p->pointer.begin() + temp);
					temp = 0;
					while(p->father->pointer[temp++] != p);
					p->father->pointer.insert(p->father->pointer.begin() + temp, t);
					temp--;
					p->father->key.insert(p->father->key.begin() + temp, p->key[degree / 2]);
					p->key.pop_back();
					p = p->father;
				}
			}
		}
	}
	else if (type == TYPE_FLOAT)
	{

	}
	else if (type == TYPE_CHAR)
	{

	}
	else
		return ERROR_UNKNOWN;

	return 0;
}

int BPLUSTREE::DeleteNode(int type, string * file_name)
{
	if (type == TYPE_INT)

	{
	}
	else if (type == TYPE_FLOAT)
	{

	}
	else if (type == TYPE_CHAR)
	{

	}
	else
		return ERROR_UNKNOWN;
	return 0;
}



int GetDegree(int block_size, int type)
{
	int degree, type_size;
	//int -> 4bytes float -> 4bytes char[] -> 255bytes
	if (type == TYPE_INT || type == TYPE_FLOAT)
		type_size = 4;
	else if (type == TYPE_CHAR)
		type_size = 255;
	else return (ERROR_TYPE);
	degree = (block_size + type_size) / (POINTERSIZE + type_size);
	return degree;
}

