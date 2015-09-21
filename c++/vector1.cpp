/*
 * =====================================================================================
 *
 *       Filename:  vector1.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年05月28日 09时49分28秒
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
	> File Name: vector1.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: 2015年05月28日 星期四 09时49分28秒
 ************************************************************************/

#include<iostream>
#include <vector>
using namespace std;

int main(){
    vector< int >vi;
    for(int i=0;i<10;i++){
        vi.push_back(i);
    }
    for(int i=0;i<10;i++){
        cout<<vi[i]<<" ";
    }
    cout<<endl;
    for(vector< int >::iterator it=vi.begin();it!=vi.end();it++){
        cout<<*it<<" ";
    }
    cout<<endl;
    return 0;
}
