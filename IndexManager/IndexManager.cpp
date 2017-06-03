#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<stdlib.h>
#include"IndexManager.h"

using namespace std;

BPLUSTREE::BPLUSTREE(int block_size):block_size(block_size)
{
	ifstream file("allindex.idx");
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
	{
		cout << "Error: No file named " << *file_name << "!" << endl;
		return ERROR_FILE;
	}
	if (type == TYPE_INT)
	{
		INTNODE *root= new INTNODE();
		root->father = NULL;
		//Create an empty root node and add it into map
		int_map[*file_name] = root;
		//cout << "Create a node";//......................................
		string key;
		int offset, block;
		while (1)
		{
			if (index.eof())
				break;
			index >> key >> block >>  offset;
			AddNode(type, file_name, &key, block, offset);
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
		cout << "Error: Type error!" << endl;
		return ERROR_UNKNOWN;
	}
	index.close();
	return 0;
}

int BPLUSTREE::CreateIndex(string * attribute, string * file_name, int type)
{
	//If there exits index on this attribute or with the same index_name
	ifstream check("allindex.idx");
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
		cout << "Error: Exits index!" << endl;
		return ERROR_INDEX_EXISTS;
	}
	check.close();
	//Add this index into manager file 
	ofstream allindex("allindex.idx", ios::app);
	
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
	cmd = "rename allindex.idx all.idx";
	system(cmd.c_str());
	
	ifstream old("all.idx");
	ofstream file("allindex.idx");
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
	cmd = "DEL all.idx";
	system(cmd.c_str());
	int_map.erase(*file_name);
	return 0;
}

int BPLUSTREE::AddNode(int type, string * file_name, string *skey,int block, int offset)
{
	int degree = GetDegree(block_size, type);
	if (type == TYPE_INT)
	{
		INTNODE *root = int_map[*file_name];
		INTNODE  *p;
		int key;
		stringstream ss(*skey);

		p = root;
		ss >> key;
		//cout << degree << endl;//...........................................
		//Add to file
		ofstream record(*file_name,ios::app);

		record << key << " " << block <<  offset << endl;
		record.close();
		//The first node
		if (root->key.size() == 0)
		{
			root->key.push_back(key);
			root->offset.push_back(offset);
			root->block.push_back(block);
			return SUCCESS;
		}
		//Add node to a non-empty tree
		while (p->offset.size() == 0)
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
		p->offset.insert(p->offset.begin() + i, offset);
		p->block.insert(p->block.begin() + i, block);
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
				if (p->offset.size() != 0)
				{
					//Copy left half to t1
					for (int i = 0; i < (degree + 1)/ 2; i++)
					{
						t1->key.push_back(p->key[i]);
						if (p->offset.size() != 0)
							t1->offset.push_back(p->offset[i]);
						if (p->block.size() != 0)
							t1->block.push_back(p->block[i]);
					}
					//Copy right half  to t2	
					for (int i = (degree + 1) / 2; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						if (p->offset.size() != 0)
							t2->offset.push_back(p->offset[i]);
						if (p->block.size() != 0)
							t2->block.push_back(p->block[i]);
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
				p->block.clear();
				p->offset.clear();
				p->pointer.clear();
				p->pointer.push_back(t1);
				p->pointer.push_back(t2);		
				break;
			}
			else
			{
				INTNODE *t;

				//Split leaf node
				if (p->offset.size() != 0)
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
						t->block.push_back(p->block[temp]);
						t->offset.push_back(p->offset[temp]);
						p->key.erase(p->key.begin() + temp);
						p->block.erase(p->block.begin() + temp);
						p->offset.erase(p->offset.begin()+temp);
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

	return SUCCESS;
}
 
int BPLUSTREE::DeleteNode(int type, string * file_name, string *skey)
{
	int degree = GetDegree(block_size, type);
	stringstream ss(*skey);
	string output;

	if (type == TYPE_INT)
	{
		int key;
		INTNODE *p, *pre=NULL, *next=NULL, *root;

		ss >> key;
		root = p = int_map[*file_name];
		//Find the leaf node which contains the key
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= key)
				i++;
			i++;
			if (p->key[0] > key) i = 0;
			if (i > 0)
				pre = p->pointer[i - 1];
			else if (i + 1 < p->key.size())
				next = p->pointer[i + 1];
			p = p->pointer[i];
		}
		//Find the key
		int i = 0;
		while (i < p->key.size() && p->key[i] != key)
			i++;
		if (i == p->key.size())
		{
			cout << "Error: No such turple!" << endl;
			return ERROR_offset_NOT_EXISTS;
		}
		output = to_string(p->block[i]) + " " + to_string(p->offset[i]);
		//Delete it from B+ tree and file
		p->key.erase(p->key.begin() + i);
		p->block.erase(p->block.begin() + i);
		p->offset.erase(p->offset.begin() + i);

		string cmd;
		cmd = "rename "+*file_name +" temp.idx";
		system(cmd.c_str());
		ifstream old("temp.idx");
		ofstream nfile(*file_name);
		int offset, tkey, block;
		while (1)
		{
			if (old.eof())
				break;
			old >> tkey >> block >> offset;
			if (key == tkey) continue;
			nfile << tkey << " " << block << offset << endl;
		}
		old.close();
		nfile.close();
		if (p->key.size() >= degree / 2) 
			return SUCCESS;
		//Adjust B+ tree
		if (!pre && (next && next->key.size() + p->key.size() < degree))
			pre = p;
		p = next;
		while (p->key.size() < degree / 2)
		{
			//If p's father is root
			if (p->father == root)
			{
				if (root->pointer.size() != 1)
					break;
				//Only son in root, p becomes the root
				int_map[*file_name] = p;
				p->father = NULL;
				free(root);
			}
			//Merge two nodes to the left node
			if (pre && pre->key.size() + p->key.size() < degree)
			{
				//Merge leaf nodes
				if (p->offset.size() == 0)
				{
					 //Copy keys and offsets to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre->key.push_back(p->key[i]);
						pre->block.push_back(p->block[i]);
						pre->offset.push_back(p->offset[i]);
					}
					pre->pointer.clear();
					if (p->pointer.size() != 0)
						pre->pointer.push_back(p->pointer[0]);
					p->key.clear();
					p->block.clear();
					p->offset.clear();
					pre = p;
					//p is the first pointer of its father
					if (p->father != pre->father)
					{
						p = p->father;
						p->key.erase(p->key.begin());
						p->pointer.erase(p->pointer.begin());
					}
					else
					{//p is not the first pointer of its father
						int i = 0;
						while (p->father->pointer[i] != p)
							i++;
						p = p->father;
						p->pointer.erase(p->pointer.begin() + i);
						i--;
						p->key.erase(p->key.begin() + i);
					}
					free(pre);
				}
				else//Merge non-leaf nodes
				{
					//Copy keys and pointers to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre->key.push_back(p->key[i]);
						pre->pointer.push_back(p->pointer[i]);
					}
					pre->pointer.push_back(p->pointer[i]);
					p->key.clear();
					p->pointer.clear();
					pre = p;
					if (p->father != pre->father)
					{//p is the first pointer of its father
						p = p->father;
						p->key.erase(p->key.begin());
						p->pointer.erase(p->pointer.begin());
					}
					else
					{//p is not the first pointer of its father
						int i = 0;
						while (p->father->pointer[i] != p)
							i++;
						p = p->father;
						p->pointer.erase(p->pointer.begin() + i);
						i--;
						p->key.erase(p->key.begin() + i);
					}
					free(pre);
				}
			}
			else//Redistribute pointers
			{
				if (p->offset.size() != 0)
				{//Leaf nodes
					if (pre)//Take one key from pre_node to p
					{
						p->key.insert(p->key.begin(), pre->key[pre->key.size() - 1]);
						p->block.insert(p->block.begin(), pre->block[pre->block.size() - 1]);
						p->offset.insert(p->offset.begin(), pre->offset[pre->offset.size() - 1]);
						pre->key.pop_back();
						pre->block.pop_back();
						pre->offset.pop_back();
					}
					else//Take one key from next_node to p
					{
						p->key.push_back(next->key[0]);
						p->block.push_back(next->block[0]);
						p->offset.push_back(next->offset[0]);
						next->key.erase(next->key.begin());
						next->block.erase(next->block.begin());
						next->offset.erase(next->offset.begin());
					}
				}
				else//Non-leaf nodes
				{
					if (pre)//Take one key from pre_node to p
					{
						p->key.insert(p->key.begin(), pre->key[pre->key.size() - 1]);
						p->pointer.insert(p->pointer.begin(), pre->pointer[pre->pointer.size() - 1]);
						pre->key.pop_back();
						pre->pointer.pop_back();
					}
					else//Take one key from next_node to p
					{
						p->key.push_back(next->key[0]);
						p->pointer.push_back(next->pointer[0]);
						next->key.erase(next->key.begin());
						next->pointer.erase(next->pointer.begin());
					}
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
	{
		cout << "Error: Unknown!" << endl;
		return ERROR_UNKNOWN;
	}	
	ofstream tempf("temp.tmp",ios::app);

	tempf << output << endl;
	tempf.close();
	return SUCCESS;
}



int GetDegree(int block_size, int type)
{
	int degree, type_size;
	//int -> 4bytes float -> 4bytes char[] -> 255bytes
	if (type == TYPE_INT || type == TYPE_FLOAT)
		type_size = 4;
	else if (type == TYPE_CHAR)
		type_size = 255;
	else
	{
		cout << "Error: No such type!" << endl;
		return ERROR_TYPE;
	}
	degree = (block_size + type_size) / (POINTERSIZE + type_size);
	//Degree >= 16
	return degree;
}

