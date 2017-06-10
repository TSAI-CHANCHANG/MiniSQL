#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<stdlib.h>
#include"IndexManager.h"

using namespace std;

BPLUSTREE::BPLUSTREE(int block_size):block_size(block_size)
{
    ifstream file("allindex.idx",ios::app);
    file.seekg(0,ios::beg);
	string file_name;
	int type;
	while (1)
	{
		file >> file_name;
		if (file.eof())
			break;
		file >> type;
		//cout<<file_name<<" "<<type;//............................
		if (type == TYPE_INT)
		{
			//Create int B+Tree and add root node pointer into map
			CreateTree(type, &file_name);
		}
		else if (type == TYPE_FLOAT)
		{
			CreateTree(type, &file_name);
			//Create float B+Tree and add root node pointer into map
		}
		else if (type == TYPE_CHAR)
		{
			CreateTree(type, &file_name);
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
		root->next = root->prev = NULL;
		//Create an empty root node and add it into map
		int_map[*file_name] = root;
		//cout << "Create a node";//......................................
		string key;
		int offset, block;
		while (1)
		{
			index >> key;
			if (index.eof())
				break;
			index >> block >>  offset;
			AddNode(type, file_name, &key, block, offset);
 		}
	}
	else if (type == TYPE_FLOAT)
	{
		FLOATNODE *root = new FLOATNODE();
		root->father = NULL;
		root->next = root->prev = NULL;
		//Create an empty root node and add it into map
		float_map[*file_name] = root;
		//cout << "Create a node";//......................................
		string key;
		int offset, block;
		while (1)
		{
			index >> key;
			if (index.eof())
				break;
			index >> block >> offset;
			AddNode(type, file_name, &key, block, offset);
		}
	}
	else if (type == TYPE_CHAR)
	{
		CHARNODE *root = new CHARNODE();
		root->father = NULL;
		root->next = root->prev = NULL;
		//Create an empty root node and add it into map
		char_map[*file_name] = root;
		//cout << "Create a node";//......................................
		string key;
		int offset, block;
		while (1)
		{
			index >> key;
			if (index.eof())
				break;
			index >> block >> offset;
			AddNode(type, file_name, &key, block, offset);
		}
	}	
	else
	{
		index.close();
		cout << "Error: Type error!" << endl;
		return ERROR_UNKNOWN;
	}
	index.close();
	return SUCCESS;
}

int BPLUSTREE::CreateIndex( string * file_name, int type)
{
	//If there exits index on this attribute or with the same index_name
	
	string s1, s2;
	int temp;

	/*ifstream check("allindex.idx");
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
	check.close();*/
	//Add this index into manager file 
	ofstream allindex("allindex.idx", ios::app);
	
	allindex << *file_name << " " << type << endl;
	allindex.close();
	//Create B+Tree
	if (CreateTree(type, file_name) < 0)
		return ERROR_UNKNOWN;
	return SUCCESS;
}

int BPLUSTREE::DropIndex(string * file_name, int type)
{
	string cmd, filen;
	
	cmd = "DEL " + *file_name;
	system(cmd.c_str());
	cmd = "rename allindex.idx all.idx";
	system(cmd.c_str());
	
	ifstream old("all.idx");
	ofstream file("allindex.idx");
	int ttype;
	while (1)
	{
		old >> filen;
		if (old.eof())
			break;
		old >> ttype;
		if (filen == *file_name) continue;
		file << filen << " " << ttype << endl;
	}
	old.close();
	file.close();
	cmd = "DEL all.idx";
	system(cmd.c_str());
	if (type == TYPE_INT)
		int_map.erase(*file_name);
	else if (type == TYPE_FLOAT)
		float_map.erase(*file_name);
	else if (type == TYPE_CHAR)
		char_map.erase(*file_name);
	else
	{
		cout << "Error: No such file!" << endl;
		return ERROR_FILE;
	}
	return SUCCESS;
}

int BPLUSTREE::Insert(int type, string * file_name, string * skey, int block, int offset)
{
	stringstream ss(*skey);
	ofstream record(*file_name, ios::app);
	int key;
	ss >> key;
	record << key << " " << block << " " << offset << endl;
	record.close();
	//Add to file
	if (AddNode(type, file_name, skey, block, offset)==SUCCESS)
		return SUCCESS;
	else return ERROR_UNKNOWN;
}

int BPLUSTREE::Delete(int type, string * file_name, string * lbound, string * rbound, int lopen, int ropen)
{//1 for open; 0 for close
	stringstream ls(*lbound), rs(*rbound);
	string cmd;
	
	cmd = "rename " + *file_name + " old.idx";
	system(cmd.c_str());
	ifstream old("old.idx");
	ofstream updated(*file_name);
	ofstream result("delete.tmp");

	if (type == TYPE_INT)
	{
		int lb, rb, key, block, offset;

		ls >> lb;
		rs >> rb;
		
		while (1)
		{
			old >> key;
			if (old.eof())
				break;
			old >> block >> offset;
			if (lopen && ropen)
				if (key < rb && key > lb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (lopen && !ropen)
				if (key > lb && key <= rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (!lopen && ropen)
				if (key >= lb && key < rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (!lopen && !ropen)
				if (key >= lb && key <= rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
		}
		old.close();
		result.close();
		updated.close();
		cmd = "del old.idx";
		system(cmd.c_str());
		int_map.erase(*file_name);
		if (CreateTree(type, file_name) == SUCCESS)
			return SUCCESS;
		else return ERROR_UNKNOWN;
	}
	else if (type == TYPE_FLOAT)
	{
		float lb, rb, key;
		int block, offset;

		ls >> lb;
		rs >> rb;

		while (1)
		{
			old >> key;
			if (old.eof())
				break;
			old >> block >> offset;
			if (lopen && ropen)
				if (key < rb && key > lb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (lopen && !ropen)
				if (key > lb && key <= rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (!lopen && ropen)
				if (key >= lb && key < rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (!lopen && !ropen)
				if (key >= lb && key <= rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
		}
		old.close();
		result.close();
		updated.close();
		cmd = "del old.idx";
		system(cmd.c_str());
		float_map.erase(*file_name);
		if (CreateTree(type, file_name) == SUCCESS)
			return SUCCESS;
		else return ERROR_UNKNOWN;
	}
	else if (type == TYPE_CHAR)
	{
		string lb, rb, key;
		int block, offset;

		lb = *lbound;
		rb = *rbound;

		while (1)
		{
			old >> key;
			if (old.eof())
				break;
			old >> block >> offset;
			if (lopen && ropen)
				if (key < rb && key > lb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (lopen && !ropen)
				if (key > lb && key <= rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (!lopen && ropen)
				if (key >= lb && key < rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
			else if (!lopen && !ropen)
				if (key >= lb && key <= rb)
					result << block << " " << offset << endl;
				else
					updated << key << " " << block << " " << offset << endl;
		}
		old.close();
		result.close();
		updated.close();
		cmd = "del old.idx";
		system(cmd.c_str());
		char_map.erase(*file_name);
		if (CreateTree(type, file_name) == SUCCESS)
			return SUCCESS;
		else return ERROR_UNKNOWN;
	}
	else return ERROR_TYPE;

	return SUCCESS;
}

int BPLUSTREE::Find(int type, string * file_name, string * lbound, string * rbound, int lopen, int ropen)
{
	stringstream ls(*lbound),rs(*rbound);
	//Clear select.tmp
	ofstream tmp("select.tmp");
	tmp.close();

	if (type == TYPE_INT)
	{
		INTNODE *p, *left, *right;
		int lb, rb, li, ri, i;

		ls >> lb;
		rs >> rb;
		//No result
		if (lb > rb || (lb == rb && (lopen || ropen))) return SUCCESS;
		p = int_map[*file_name];
		li = 0;
		left = right = NULL;
		//Find the left node
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= lb)
				i++;
			i++;
			if (p->key[0] > lb) i = 0;
			p = p->pointer[i];
		}
		left = p;
		if (lopen)//Left open
		{
			while (left->key[li] <= lb)
			{
				li++;
				if (li >= left->key.size())
				{
					li = 0;
					if (left->next)
						left = left->next;
					else//No result
						return SUCCESS;
				}
			}
		}
		else//Left close
		{
			while (left->key[li] < lb)
			{
				li++;
				if (li >= left->key.size())
				{
					li = 0;
					if (left->next)
						left = left->next;
					else//No result
						return SUCCESS;
				}
			}
		}
		//Find the right node
		p = int_map[*file_name];
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= rb)
				i++;
			i++;
			if (p->key[0] > rb) i = 0;
			p = p->pointer[i];
		}
		right = p;
		ri = right->key.size()-1;
		if (ropen)//Right open
		{
			while (right->key[ri] >= rb)
			{
				ri--;
				if (ri < 0)
				{
					if (right->prev)
						right = right->prev;
					else//No result
						return SUCCESS;
					ri = right->key.size() - 1;
				}
			}
		}
		else//Right close
		{
			while (right->key[ri] > rb)
			{
				ri--;
				if (ri < 0)
				{
					if (right->prev)
						right = right->prev;
					else//No result
						return SUCCESS;
					ri = right->key.size() - 1;
				}
			}
		}
		p = left;
		i = li;
		ofstream tmp("select.tmp");
		while (1)
		{		
			if ((p == right && i > ri) || (right->next == left))
				break;
			tmp << p->block[i] << " " << p->offset[i] << endl;		
			if (p == right && i == ri)
				break;
			i++;
			if (i == p->key.size())
			{
				i = 0;
				p = p->next;
			}
		}
		tmp.close();
	}
	else if (type == TYPE_FLOAT)
	{
		FLOATNODE *p, *left, *right;
		float lb, rb;
		int li, ri, i;

		ls >> lb;
		rs >> rb;
		//No result
		if (lb > rb || (lb == rb && (lopen || ropen))) return SUCCESS;
		p = float_map[*file_name];
		li = 0;
		left = right = NULL;
		//Find the left node
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= lb)
				i++;
			i++;
			if (p->key[0] > lb) i = 0;
			p = p->pointer[i];
		}
		left = p;
		if (lopen)//Left open
		{
			while (left->key[li] <= lb)
			{
				li++;
				if (li >= left->key.size())
				{
					li = 0;
					if (left->next)
						left = left->next;
					else//No result
						return SUCCESS;
				}
			}
		}
		else//Left close
		{
			while (left->key[li] < lb)
			{
				li++;
				if (li >= left->key.size())
				{
					li = 0;
					if (left->next)
						left = left->next;
					else//No result
						return SUCCESS;
				}
			}
		}
		//Find the right node
		p = float_map[*file_name];
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= rb)
				i++;
			i++;
			if (p->key[0] > rb) i = 0;
			p = p->pointer[i];
		}
		right = p;
		ri = right->key.size() - 1;
		if (ropen)//Right open
		{
			while (right->key[ri] >= rb)
			{
				ri--;
				if (ri < 0)
				{
					if (right->prev)
						right = right->prev;
					else//No result
						return SUCCESS;
					ri = right->key.size() - 1;
				}
			}
		}
		else//Right close
		{
			while (right->key[ri] > rb)
			{
				ri--;
				if (ri < 0)
				{
					if (right->prev)
						right = right->prev;
					else//No result
						return SUCCESS;
					ri = right->key.size() - 1;
				}
			}
		}
		p = left;
		i = li;
		ofstream tmp("select.tmp");
		while (1)
		{
			if ((p == right && i > ri) || (right->next == left))
				break;
			tmp << p->block[i] << " " << p->offset[i] << endl;
			if (p == right && i == ri)
				break;
			i++;
			if (i == p->key.size())
			{
				i = 0;
				p = p->next;
			}
		}
		tmp.close();
	}
	else if (type == TYPE_CHAR)
	{
		CHARNODE *p, *left, *right;
		string lb, rb;
		int li, ri, i;

		ls >> lb;
		rs >> rb;
		//No result
		if (lb > rb || (lb == rb && (lopen || ropen))) return SUCCESS;
		p = char_map[*file_name];
		li = 0;
		left = right = NULL;
		//Find the left node
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= lb)
				i++;
			i++;
			if (p->key[0] > lb) i = 0;
			p = p->pointer[i];
		}
		left = p;
		if (lopen)//Left open
		{
			while (left->key[li] <= lb)
			{
				li++;
				if (li >= left->key.size())
				{
					li = 0;
					if (left->next)
						left = left->next;
					else//No result
						return SUCCESS;
				}
			}
		}
		else//Left close
		{
			while (left->key[li] < lb)
			{
				li++;
				if (li >= left->key.size())
				{
					li = 0;
					if (left->next)
						left = left->next;
					else//No result
						return SUCCESS;
				}
			}
		}
		//Find the right node
		p = char_map[*file_name];
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= rb)
				i++;
			i++;
			if (p->key[0] > rb) i = 0;
			p = p->pointer[i];
		}
		right = p;
		ri = right->key.size() - 1;
		if (ropen)//Right open
		{
			while (right->key[ri] >= rb)
			{
				ri--;
				if (ri < 0)
				{
					if (right->prev)
						right = right->prev;
					else//No result
						return SUCCESS;
					ri = right->key.size() - 1;
				}
			}
		}
		else//Right close
		{
			while (right->key[ri] > rb)
			{
				ri--;
				if (ri < 0)
				{
					if (right->prev)
						right = right->prev;
					else//No result
						return SUCCESS;
					ri = right->key.size() - 1;
				}
			}
		}
		p = left;
		i = li;
		ofstream tmp("select.tmp");
		while (1)
		{
			if ((p == right && i > ri) || (right->next == left))
				break;
			tmp << p->block[i] << " " << p->offset[i] << endl;
			if (p == right && i == ri)
				break;
			i++;
			if (i == p->key.size())
			{
				i = 0;
				p = p->next;
			}
		}
		tmp.close();
	}
	else
	{
		cout << "Error: No such type!" << endl;
		return ERROR_TYPE;
	}

	return SUCCESS;
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
		{
			p->key.insert(p->key.begin() + i, key);
			p->offset.insert(p->offset.begin() + i, offset);
			p->block.insert(p->block.begin() + i, block);
		}	
		else
		{
			p->key.push_back(key);
			p->block.push_back(block);
			p->offset.push_back(offset);
		}
		//Split from leaf
		while (p->key.size() == degree)
		{
			//Split root
			if (p == root)
			{
				INTNODE *t1, *t2;
		
				t2 = new INTNODE;
				t1 = new INTNODE;
				t1->prev = t2->next = t1->next = t2->prev = NULL;
				//Only root
				if (p->offset.size() != 0)
				{
					//Copy left half to t1
					for (int i = 0; i < (degree + 1)/ 2; i++)
					{
						t1->key.push_back(p->key[i]);
						t1->offset.push_back(p->offset[i]);
						t1->block.push_back(p->block[i]);
					}
					//Copy right half  to t2	
					for (int i = (degree + 1) / 2; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						t2->offset.push_back(p->offset[i]);
						t2->block.push_back(p->block[i]);
					}
					//Link t1 and t2 as they are leaves
					t1->next = t2;
					t2->prev = t1;
					p->key.clear();
					p->key.push_back(t2->key[0]);
				}
				else
				{//Root is not leaf
					//Copy left half to t1
					for (int i = 0; i < degree / 2; i++)
					{
						t1->key.push_back(p->key[i]);
						t1->pointer.push_back(p->pointer[i]);
						p->pointer[i]->father = t1;
					}
					p->pointer[degree / 2]->father = t1;
					t1->pointer.push_back(p->pointer[degree / 2]);
					//Copy right half  to t2
					for (int i = degree / 2 + 1; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						t2->pointer.push_back(p->pointer[i]);
						p->pointer[i]->father = t2;
					}
					t2->pointer.push_back(p->pointer[degree]);
					p->pointer[degree]->father = t2;
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
					t->next = p->next;
					p->next = t;
					if (t->next)
						t->next->prev = t;
					t->prev = p;
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
					if (temp != p->pointer.size())
					{
						p->pointer.insert(p->pointer.begin() + temp, t);
						p->key.insert(p->key.begin() + temp - 1, t->key[0]);
					}
					else
					{
						p->pointer.push_back(t);
						p->key.push_back(t->key[0]);
					}
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
						p->pointer[temp]->father = t;
						p->key.erase(p->key.begin() + temp);
						p->pointer.erase(p->pointer.begin()+temp);
					}
					t->pointer.push_back(p->pointer[temp]);
					p->pointer[temp]->father = t;
					p->pointer.erase(p->pointer.begin() + temp);
					temp = 0;
					while(p->father->pointer[temp++] != p);
				/*	p->father->pointer.insert(p->father->pointer.begin() + temp, t);
					temp--;
					p->father->key.insert(p->father->key.begin() + temp, p->key[degree / 2]);
					p->key.pop_back();
					p = p->father;*/
					//&)*%^)&(@^*(_$&^_*(&^_*(%^&*(@^$&%^&%_^$*(@&^*(!^*(@*(%$&@(&$@)(^$)*^$)(@*^_&
					if (temp != p->father->pointer.size())
					{
						p->father->pointer.insert(p->father->pointer.begin() + temp, t);
						p->father->key.insert(p->father->key.begin() + temp - 1, p->key[degree / 2]);
					}
					else
					{
						p->father->pointer.push_back(t);
						p->father->key.push_back(p->key[degree / 2]);
					}
					p->key.pop_back();
					p = p->father;
					//&)*%^)&(@^*(_$&^_*(&^_*(%^&*(@^$&%^&%_^$*(@&^*(!^*(@*(%$&@(&$@)(^$)*^$)(@*^_&
				}
			}
		}
	}
	else if (type == TYPE_FLOAT)
	{
		FLOATNODE *root = float_map[*file_name];
		FLOATNODE  *p;
		float key;
		stringstream ss(*skey);

		p = root;
		ss >> key;
		//cout << degree << endl;//...........................................			
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
		while (i + 1 < p->key.size() && p->key[i] <= key) i++;
		if (p->key[i] > key)
		{
			p->key.insert(p->key.begin() + i, key);
			p->offset.insert(p->offset.begin() + i, offset);
			p->block.insert(p->block.begin() + i, block);
		}
		else
		{
			p->key.push_back(key);
			p->block.push_back(block);
			p->offset.push_back(offset);
		}
		//Split from leaf
		while (p->key.size() == degree)
		{
			//Split root
			if (p == root)
			{
				FLOATNODE *t1, *t2;

				t2 = new FLOATNODE;
				t1 = new FLOATNODE;
				t1->prev = t2->next = t1->next = t2->prev = NULL;
				//Only root
				if (p->offset.size() != 0)
				{
					//Copy left half to t1
					for (int i = 0; i < (degree + 1) / 2; i++)
					{
						t1->key.push_back(p->key[i]);
						t1->offset.push_back(p->offset[i]);
						t1->block.push_back(p->block[i]);
					}
					//Copy right half  to t2	
					for (int i = (degree + 1) / 2; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						t2->offset.push_back(p->offset[i]);
						t2->block.push_back(p->block[i]);
					}
					//Link t1 and t2 as they are leaves
					t1->next = t2;
					t2->prev = t1;
					p->key.clear();
					p->key.push_back(t2->key[0]);
				}
				else
				{//Root is not leaf
				 //Copy left half to t1
					for (int i = 0; i < degree / 2; i++)
					{
						t1->key.push_back(p->key[i]);
						t1->pointer.push_back(p->pointer[i]);
						p->pointer[i]->father = t1;
					}
					p->pointer[degree / 2]->father = t1;
					t1->pointer.push_back(p->pointer[degree / 2]);
					//Copy right half  to t2
					for (int i = degree / 2 + 1; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						t2->pointer.push_back(p->pointer[i]);
						p->pointer[i]->father = t2;
					}
					t2->pointer.push_back(p->pointer[degree]);
					p->pointer[degree]->father = t2;
					//Add one key to root
					float temp = p->key[degree / 2];
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
				FLOATNODE *t;

				//Split leaf node
				if (p->offset.size() != 0)
				{
					t = new FLOATNODE;
					t->father = p->father;
					t->next = p->next;
					p->next = t;
					if (t->next)
						t->next->prev = t;
					t->prev = p;
					//Cut [m/2] to m-1 to t
					int temp = (degree + 1) / 2;
					while (p->key.size() > temp)
					{
						t->key.push_back(p->key[temp]);
						t->block.push_back(p->block[temp]);
						t->offset.push_back(p->offset[temp]);
						p->key.erase(p->key.begin() + temp);
						p->block.erase(p->block.begin() + temp);
						p->offset.erase(p->offset.begin() + temp);
					}
					//Add t to their father's pointer list
					temp = 0;
					while (p->father->pointer[temp] != p)
						temp++;
					p = p->father;
					temp++;
					if (temp != p->pointer.size())
					{
						p->pointer.insert(p->pointer.begin() + temp, t);
						p->key.insert(p->key.begin() + temp - 1, t->key[0]);
					}
					else
					{
						p->pointer.push_back(t);
						p->key.push_back(t->key[0]);
					}
				}
				//Split non-leaf node
				else
				{
					t = new FLOATNODE;
					t->father = p->father;
					int temp = degree / 2 + 1;
					while (p->key.size() > temp)
					{
						t->key.push_back(p->key[temp]);
						t->pointer.push_back(p->pointer[temp]);
						p->pointer[temp]->father = t;
						p->key.erase(p->key.begin() + temp);
						p->pointer.erase(p->pointer.begin() + temp);
					}
					t->pointer.push_back(p->pointer[temp]);
					p->pointer[temp]->father = t;
					p->pointer.erase(p->pointer.begin() + temp);
					temp = 0;
					while (p->father->pointer[temp++] != p);
					if (temp != p->father->pointer.size())
					{
						p->father->pointer.insert(p->father->pointer.begin() + temp, t);
						p->father->key.insert(p->father->key.begin() + temp - 1, p->key[degree / 2]);
					}
					else
					{
						p->father->pointer.push_back(t);
						p->father->key.push_back(p->key[degree / 2]);
					}
					p->key.pop_back();
					p = p->father;
				}
			}
		}
	}
	else if (type == TYPE_CHAR)
	{
		CHARNODE *root = char_map[*file_name];
		CHARNODE  *p;
		string key;
		stringstream ss(*skey);

		p = root;
		ss >> key;
		//cout << degree << endl;//...........................................			
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
		while (i + 1 < p->key.size() && p->key[i] <= key) i++;
		if (p->key[i] > key)
		{
			p->key.insert(p->key.begin() + i, key);
			p->offset.insert(p->offset.begin() + i, offset);
			p->block.insert(p->block.begin() + i, block);
		}
		else
		{
			p->key.push_back(key);
			p->block.push_back(block);
			p->offset.push_back(offset);
		}
		//Split from leaf
		while (p->key.size() == degree)
		{
			//Split root
			if (p == root)
			{
				CHARNODE *t1, *t2;

				t2 = new CHARNODE;
				t1 = new CHARNODE;
				t1->prev = t2->next = t1->next = t2->prev = NULL;
				//Only root
				if (p->offset.size() != 0)
				{
					//Copy left half to t1
					for (int i = 0; i < (degree + 1) / 2; i++)
					{
						t1->key.push_back(p->key[i]);
						t1->offset.push_back(p->offset[i]);
						t1->block.push_back(p->block[i]);
					}
					//Copy right half  to t2	
					for (int i = (degree + 1) / 2; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						t2->offset.push_back(p->offset[i]);
						t2->block.push_back(p->block[i]);
					}
					//Link t1 and t2 as they are leaves
					t1->next = t2;
					t2->prev = t1;
					p->key.clear();
					p->key.push_back(t2->key[0]);
				}
				else
				{//Root is not leaf
				 //Copy left half to t1
					for (int i = 0; i < degree / 2; i++)
					{
						t1->key.push_back(p->key[i]);
						t1->pointer.push_back(p->pointer[i]);
						p->pointer[i]->father = t1;
					}
					p->pointer[degree / 2]->father = t1;
					t1->pointer.push_back(p->pointer[degree / 2]);
					//Copy right half  to t2
					for (int i = degree / 2 + 1; i < degree; i++)
					{
						t2->key.push_back(p->key[i]);
						t2->pointer.push_back(p->pointer[i]);
						p->pointer[i]->father = t2;
					}
					t2->pointer.push_back(p->pointer[degree]);
					p->pointer[degree]->father = t2;
					//Add one key to root
					string temp = p->key[degree / 2];
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
				CHARNODE *t;

				//Split leaf node
				if (p->offset.size() != 0)
				{
					t = new CHARNODE;
					t->father = p->father;
					t->next = p->next;
					p->next = t;
					if (t->next)
						t->next->prev = t;
					t->prev = p;
					//Cut [m/2] to m-1 to t
					int temp = (degree + 1) / 2;
					while (p->key.size() > temp)
					{
						t->key.push_back(p->key[temp]);
						t->block.push_back(p->block[temp]);
						t->offset.push_back(p->offset[temp]);
						p->key.erase(p->key.begin() + temp);
						p->block.erase(p->block.begin() + temp);
						p->offset.erase(p->offset.begin() + temp);
					}
					//Add t to their father's pointer list
					temp = 0;
					while (p->father->pointer[temp] != p)
						temp++;
					p = p->father;
					temp++;
					if (temp != p->pointer.size())
					{
						p->pointer.insert(p->pointer.begin() + temp, t);
						p->key.insert(p->key.begin() + temp - 1, t->key[0]);
					}
					else
					{
						p->pointer.push_back(t);
						p->key.push_back(t->key[0]);
					}
				}
				//Split non-leaf node
				else
				{
					t = new CHARNODE;
					t->father = p->father;
					int temp = degree / 2 + 1;
					while (p->key.size() > temp)
					{
						t->key.push_back(p->key[temp]);
						t->pointer.push_back(p->pointer[temp]);
						p->pointer[temp]->father = t;
						p->key.erase(p->key.begin() + temp);
						p->pointer.erase(p->pointer.begin() + temp);
					}
					t->pointer.push_back(p->pointer[temp]);
					p->pointer[temp]->father = t;
					p->pointer.erase(p->pointer.begin() + temp);
					temp = 0;
					while (p->father->pointer[temp++] != p);
					if (temp != p->father->pointer.size())
					{
						p->father->pointer.insert(p->father->pointer.begin() + temp, t);
						p->father->key.insert(p->father->key.begin() + temp - 1, p->key[degree / 2]);
					}
					else
					{
						p->father->pointer.push_back(t);
						p->father->key.push_back(p->key[degree / 2]);
					}
					p->key.pop_back();
					p = p->father;
				}
			}
		}
	}
	else
	{
		cout << "Error: No such type!" << endl;
		return ERROR_TYPE;
	}

	return SUCCESS;
}
 
int BPLUSTREE::DeleteNode(int type, string * file_name, string *skey)
{
	int degree =  GetDegree(block_size, type);
	stringstream ss(*skey);
	string output;

	if (type == TYPE_INT)
	{
		int key;
		INTNODE *p, *pre_node=NULL, *next_node=NULL, *root;

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
				pre_node = p->pointer[i - 1];
			else if (i + 1 < p->key.size())
				next_node = p->pointer[i + 1];
			p = p->pointer[i];
		}
		//Find the key
		int i = 0;
		while (i < p->key.size() && p->key[i] != key)
			i++;
		if (i == p->key.size())
		{
			cout << "Error: No such turple!" << endl;
			return ERROR_OFFSET_NOT_EXISTS;
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
		int toffset, tkey, tblock;
		while (1)
		{
			old >> tkey;
			if (old.eof())
				break;
			old>> tblock >> toffset;
			if (key == tkey) continue;
			nfile << tkey << " " << tblock << " " << toffset << endl;
		}
		old.close();
		nfile.close();
		cmd = "del temp.idx";
		system(cmd.c_str());
		if (p->key.size() >= degree / 2 || p == root)
			goto end;
		//Adjust B+ tree
		if (!pre_node && (next_node && next_node->key.size() + p->key.size() < degree))
			pre_node = p;
		p = next_node;
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
			if (pre_node && pre_node->key.size() + p->key.size() < degree)
			{
				//Merge leaf nodes
				if (p->offset.size() == 0)
				{
					 //Copy keys and offsets to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre_node->key.push_back(p->key[i]);
						pre_node->block.push_back(p->block[i]);
						pre_node->offset.push_back(p->offset[i]);
					}
					pre_node->next = p->next;
					if (p->next)
						p->next->prev = pre_node;
					p->key.clear();
					p->block.clear();
					p->offset.clear();
					pre_node = p;
					//p is the first pointer of its father
					if (p->father != pre_node->father)
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
					free(pre_node);
				}
				else//Merge non-leaf nodes
				{
					//Copy keys and pointers to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre_node->key.push_back(p->key[i]);
						pre_node->pointer.push_back(p->pointer[i]);
					}
					pre_node->pointer.push_back(p->pointer[i]);
					p->key.clear();
					p->pointer.clear();
					pre_node = p;
					if (p->father != pre_node->father)
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
					free(pre_node);
				}
			}
			else//Redistribute pointers
			{
				if (p->offset.size() != 0)
				{//Leaf nodes
					if (pre_node)//Take one key from pre_node_node to p
					{
						p->key.insert(p->key.begin(), pre_node->key[pre_node->key.size() - 1]);
						p->block.insert(p->block.begin(), pre_node->block[pre_node->block.size() - 1]);
						p->offset.insert(p->offset.begin(), pre_node->offset[pre_node->offset.size() - 1]);
						pre_node->key.pop_back();
						pre_node->block.pop_back();
						pre_node->offset.pop_back();
					}
					else//Take one key from next_node_node to p
					{
						p->key.push_back(next_node->key[0]);
						p->block.push_back(next_node->block[0]);
						p->offset.push_back(next_node->offset[0]);
						next_node->key.erase(next_node->key.begin());
						next_node->block.erase(next_node->block.begin());
						next_node->offset.erase(next_node->offset.begin());
					}
				}
				else//Non-leaf nodes
				{
					if (pre_node)//Take one key from pre_node_node to p
					{
						p->key.insert(p->key.begin(), pre_node->key[pre_node->key.size() - 1]);
						p->pointer.insert(p->pointer.begin(), pre_node->pointer[pre_node->pointer.size() - 1]);
						pre_node->key.pop_back();
						pre_node->pointer.pop_back();
					}
					else//Take one key from next_node_node to p
					{
						p->key.push_back(next_node->key[0]);
						p->pointer.push_back(next_node->pointer[0]);
						next_node->key.erase(next_node->key.begin());
						next_node->pointer.erase(next_node->pointer.begin());
					}
				}
			}
		}
	}
	else if (type == TYPE_FLOAT)
	{
		float key;
		FLOATNODE *p, *pre_node = NULL, *next_node = NULL, *root;

		ss >> key;
		root = p = float_map[*file_name];
		//Find the leaf node which contains the key
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= key)
				i++;
			i++;
			if (p->key[0] > key) i = 0;
			if (i > 0)
				pre_node = p->pointer[i - 1];
			else if (i + 1 < p->key.size())
				next_node = p->pointer[i + 1];
			p = p->pointer[i];
		}
		//Find the key
		int i = 0;
		while (i < p->key.size() && p->key[i] != key)
			i++;
		if (i == p->key.size())
		{
			cout << "Error: No such turple!" << endl;
			return ERROR_OFFSET_NOT_EXISTS;
		}
		output = to_string(p->block[i]) + " " + to_string(p->offset[i]);
		//Delete it from B+ tree and file
		p->key.erase(p->key.begin() + i);
		p->block.erase(p->block.begin() + i);
		p->offset.erase(p->offset.begin() + i);

		string cmd;
		cmd = "rename " + *file_name + " temp.idx";
		system(cmd.c_str());
		ifstream old("temp.idx");
		ofstream nfile(*file_name);
		int toffset, tblock;
		float tkey;
		while (1)
		{
			old >> tkey;
			if (old.eof())
				break;
			old >> tblock >> toffset;
			if (key == tkey) continue;
			nfile << tkey << " " << tblock << " " << toffset << endl;
		}
		old.close();
		nfile.close();
		cmd = "del temp.idx";
		system(cmd.c_str());
		if (p->key.size() >= degree / 2 || p == root)
			goto end;
		//Adjust B+ tree
		if (!pre_node && (next_node && next_node->key.size() + p->key.size() < degree))
			pre_node = p;
		p = next_node;
		while (p->key.size() < degree / 2)
		{
			//If p's father is root
			if (p->father == root)
			{
				if (root->pointer.size() != 1)
					break;
				//Only son in root, p becomes the root
				float_map[*file_name] = p;
				p->father = NULL;
				free(root);
			}
			//Merge two nodes to the left node
			if (pre_node && pre_node->key.size() + p->key.size() < degree)
			{
				//Merge leaf nodes
				if (p->offset.size() == 0)
				{
					//Copy keys and offsets to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre_node->key.push_back(p->key[i]);
						pre_node->block.push_back(p->block[i]);
						pre_node->offset.push_back(p->offset[i]);
					}
					pre_node->next = p->next;
					if (p->next)
						p->next->prev = pre_node;
					p->key.clear();
					p->block.clear();
					p->offset.clear();
					pre_node = p;
					//p is the first pointer of its father
					if (p->father != pre_node->father)
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
					free(pre_node);
				}
				else//Merge non-leaf nodes
				{
					//Copy keys and pointers to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre_node->key.push_back(p->key[i]);
						pre_node->pointer.push_back(p->pointer[i]);
					}
					pre_node->pointer.push_back(p->pointer[i]);
					p->key.clear();
					p->pointer.clear();
					pre_node = p;
					if (p->father != pre_node->father)
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
					free(pre_node);
				}
			}
			else//Redistribute pointers
			{
				if (p->offset.size() != 0)
				{//Leaf nodes
					if (pre_node)//Take one key from pre_node_node to p
					{
						p->key.insert(p->key.begin(), pre_node->key[pre_node->key.size() - 1]);
						p->block.insert(p->block.begin(), pre_node->block[pre_node->block.size() - 1]);
						p->offset.insert(p->offset.begin(), pre_node->offset[pre_node->offset.size() - 1]);
						pre_node->key.pop_back();
						pre_node->block.pop_back();
						pre_node->offset.pop_back();
					}
					else//Take one key from next_node_node to p
					{
						p->key.push_back(next_node->key[0]);
						p->block.push_back(next_node->block[0]);
						p->offset.push_back(next_node->offset[0]);
						next_node->key.erase(next_node->key.begin());
						next_node->block.erase(next_node->block.begin());
						next_node->offset.erase(next_node->offset.begin());
					}
				}
				else//Non-leaf nodes
				{
					if (pre_node)//Take one key from pre_node_node to p
					{
						p->key.insert(p->key.begin(), pre_node->key[pre_node->key.size() - 1]);
						p->pointer.insert(p->pointer.begin(), pre_node->pointer[pre_node->pointer.size() - 1]);
						pre_node->key.pop_back();
						pre_node->pointer.pop_back();
					}
					else//Take one key from next_node_node to p
					{
						p->key.push_back(next_node->key[0]);
						p->pointer.push_back(next_node->pointer[0]);
						next_node->key.erase(next_node->key.begin());
						next_node->pointer.erase(next_node->pointer.begin());
					}
				}
			}
		}
	}
	else if (type == TYPE_CHAR)
	{
		string key;
		CHARNODE *p, *pre_node = NULL, *next_node = NULL, *root;

		ss >> key;
		root = p = char_map[*file_name];
		//Find the leaf node which contains the key
		while (p->offset.size() == 0)
		{
			int i = 0;
			while (i + 1 < p->key.size() && p->key[i + 1] <= key)
				i++;
			i++;
			if (p->key[0] > key) i = 0;
			if (i > 0)
				pre_node = p->pointer[i - 1];
			else if (i + 1 < p->key.size())
				next_node = p->pointer[i + 1];
			p = p->pointer[i];
		}
		//Find the key
		int i = 0;
		while (i < p->key.size() && p->key[i] != key)
			i++;
		if (i == p->key.size())
		{
			cout << "Error: No such turple!" << endl;
			return ERROR_OFFSET_NOT_EXISTS;
		}
		output = to_string(p->block[i]) + " " + to_string(p->offset[i]);
		//Delete it from B+ tree and file
		p->key.erase(p->key.begin() + i);
		p->block.erase(p->block.begin() + i);
		p->offset.erase(p->offset.begin() + i);

		string cmd;
		cmd = "rename " + *file_name + " temp.idx";
		system(cmd.c_str());
		ifstream old("temp.idx");
		ofstream nfile(*file_name);
		int toffset, tblock;
		string tkey;
		while (1)
		{
			old >> tkey;
			if (old.eof())
				break;
			old >> tblock >> toffset;
			if (key == tkey) continue;
			nfile << tkey << " " << tblock << " " << toffset << endl;
		}
		old.close();
		nfile.close();
		cmd = "del temp.idx";
		system(cmd.c_str());
		if (p->key.size() >= degree / 2 || p == root)
			goto end;
		//Adjust B+ tree
		if (!pre_node && (next_node && next_node->key.size() + p->key.size() < degree))
			pre_node = p;
		p = next_node;
		while (p->key.size() < degree / 2)
		{
			//If p's father is root
			if (p->father == root)
			{
				if (root->pointer.size() != 1)
					break;
				//Only son in root, p becomes the root
				char_map[*file_name] = p;
				p->father = NULL;
				free(root);
			}
			//Merge two nodes to the left node
			if (pre_node && pre_node->key.size() + p->key.size() < degree)
			{
				//Merge leaf nodes
				if (p->offset.size() == 0)
				{
					//Copy keys and offsets to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre_node->key.push_back(p->key[i]);
						pre_node->block.push_back(p->block[i]);
						pre_node->offset.push_back(p->offset[i]);
					}
					pre_node->next = p->next;
					if (p->next)
						p->next->prev = pre_node;
					p->key.clear();
					p->block.clear();
					p->offset.clear();
					pre_node = p;
					//p is the first pointer of its father
					if (p->father != pre_node->father)
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
					free(pre_node);
				}
				else//Merge non-leaf nodes
				{
					//Copy keys and pointers to the left node
					for (int i = 0; i < p->key.size(); i++)
					{
						pre_node->key.push_back(p->key[i]);
						pre_node->pointer.push_back(p->pointer[i]);
					}
					pre_node->pointer.push_back(p->pointer[i]);
					p->key.clear();
					p->pointer.clear();
					pre_node = p;
					if (p->father != pre_node->father)
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
					free(pre_node);
				}
			}
			else//Redistribute pointers
			{
				if (p->offset.size() != 0)
				{//Leaf nodes
					if (pre_node)//Take one key from pre_node_node to p
					{
						p->key.insert(p->key.begin(), pre_node->key[pre_node->key.size() - 1]);
						p->block.insert(p->block.begin(), pre_node->block[pre_node->block.size() - 1]);
						p->offset.insert(p->offset.begin(), pre_node->offset[pre_node->offset.size() - 1]);
						pre_node->key.pop_back();
						pre_node->block.pop_back();
						pre_node->offset.pop_back();
					}
					else//Take one key from next_node_node to p
					{
						p->key.push_back(next_node->key[0]);
						p->block.push_back(next_node->block[0]);
						p->offset.push_back(next_node->offset[0]);
						next_node->key.erase(next_node->key.begin());
						next_node->block.erase(next_node->block.begin());
						next_node->offset.erase(next_node->offset.begin());
					}
				}
				else//Non-leaf nodes
				{
					if (pre_node)//Take one key from pre_node_node to p
					{
						p->key.insert(p->key.begin(), pre_node->key[pre_node->key.size() - 1]);
						p->pointer.insert(p->pointer.begin(), pre_node->pointer[pre_node->pointer.size() - 1]);
						pre_node->key.pop_back();
						pre_node->pointer.pop_back();
					}
					else//Take one key from next_node_node to p
					{
						p->key.push_back(next_node->key[0]);
						p->pointer.push_back(next_node->pointer[0]);
						next_node->key.erase(next_node->key.begin());
						next_node->pointer.erase(next_node->pointer.begin());
					}
				}
			}
		}
	}
	else
	{
		cout << "Error: Unknown!" << endl;
		return ERROR_UNKNOWN;
	}	
end:
	ofstream tempf("delete.tmp");

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
	return degree;//#################################################################DEBUG
}

