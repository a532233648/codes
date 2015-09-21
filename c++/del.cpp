/*
 * =====================================================================================
 *
 *       Filename:  del.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年05月28日 11时24分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
/*************************************************************************
	> File Name: del.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年05月28日 星期四 11时24分31秒
 ************************************************************************/

#include<iostream>
using namespace std;

class T{
    public:
        T(){cout<<"constrcutor"<<endl;}
        ~T(){cout<<"destcutor"<<endl;}
};

int main(){
    const int NUM=3;
    T *p1=new T[NUM];
    cout<<hex<<p1<<endl;
    //delete[] p1;
    delete p1;

   // T *p2=new T[NUM];
    //cout<<p2<<endl;
    //delete[] p2;
    //return 0;
}
