#include"IndexManager.h"

using namespace std;

int main()
{
	//ifstream ini("init.txt");
	BPLUSTREE bt(4096);
	string s1 = "int1", s2 = "int2.txt", f="int2.txt";

	//bt.CreateIndex(&s1 , &s2, 1);
	bt.DropIndex(&f);

	getchar();
}