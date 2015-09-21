/*************************************************************************
	> File Name: rfinf.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月31日 星期五 15时14分57秒
 ************************************************************************/

#include<iostream>
#include <cstring>
#include <string>
using namespace std;
int main()
{
	int loc;
    string s = "My cat's breath smells like cat food.";

	loc = s.rfind( "breath" );
	cout << "The word breath is at index " << loc << std::endl;
	//-1
    loc = s.rfind( "breath", 9 );
	cout << "The word breath is at index " << loc << std::endl;
	//9

    loc = s.rfind( "breath", 20 );
	cout << "The word breath is at index " << loc << std::endl;
	//9

	string ss = "Hi Bill, I'm ill, so please pay the bill";
	loc = ss.rfind("ill");
	cout << "The word breath is at index " << loc << std::endl;
	//37
	loc = ss.rfind("ill",20);
	cout << "The word breath is at index " << loc << std::endl;
	//13
    return 0;
}
