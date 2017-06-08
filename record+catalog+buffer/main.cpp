#include "catalogmanager.h"
#include "IndexManager.h"
#include "defination.h"
#include <iostream>

extern block blocks[BLOCKNUMBER];

int main()
{
    catalogmanager cat;
    BPLUSTREE BT(BLOCKSIZE);
    cout<<cat.CreateTable("tab","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;

    cout<<cat.CreateTable("tab1","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;

    cout<<cat.CreateTable("tab2","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;

    cout<<cat.CreateTable("tab3","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    cout<<cat.CreateTable("tab4","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    cout<<cat.CreateTable("tab5","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    cout<<cat.CreateTable("tab6","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    cout<<cat.CreateTable("tab7","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    cout<<cat.CreateTable("tab8","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    cout<<cat.CreateTable("tab9","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    cout<<cat.CreateTable("tab10","ID int,Name   char(35) \n  unique ,   "
                      "CountryCode  char(3),\nDistrict char(20),Population"
                       " int,primary     key(ID)",BT)<<endl;
    getchar();
        cout<<cat.CreateIndex("HHH","tab","Name",BT);
    getchar();
    cout<<cat.DropIndex("ID","tab",BT);
    getchar();
    cout<<cat.DropTable("tab",BT);

    /*buffermanager buf;
    int num;
    for (int i = 0;i<20;i++)
    {
        num = buf.FindBlockinBuffer("1.tab", i);
        buf.Insert(num,0,"ABCDE");
        buf.DirtBlock(num);
        cout<<num;
        char ch;
        cin>>ch;
    }*/
    //cout<<buf.FindBlockinBuffer("1.tab",18);

    /*int offset;
    buf.FindSuitBlockinBuffer("1.tab",5,&num,&offset);
    cout<<num<<" "<<offset<<endl;
    buf.FindSuitBlockinBuffer("1.tab",4094,&num,&offset);
    cout<<num<<" "<<offset<<endl;
    cout<<blocks[num].fileoffset<<endl;
    buf.Delete(0,0,10);
    buf.FindSuitBlockinBuffer("1.tab",4094,&num,&offset);
    cout<<num<<" "<<offset<<endl;
    cout<<blocks[num].fileoffset;*/

    /*int num;
    int offset;
    buf.FindSuitBlockinBuffer("tab.tab",7,&num,&offset);
    cout<<num<<" "<<offset<<endl;*/
}
