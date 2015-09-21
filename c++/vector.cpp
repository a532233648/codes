/*************************************************************************
	> File Name: vector.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月28日 星期二 10时36分20秒
 ************************************************************************/

#include<iostream>
#include<vector>
#include<string>
using namespace std;
int main(){
    char ip[11][1024]={"0.0.0.255","25.0.0.255","50.0.0.255","75.0.0.255","100.0.0.255","125.0.0.255","150.0.0.255","175.0.0.255","200.0.0.255","225.0.0.255","250.0.0.255"};
    vector<string>visited(ip,ip+11);
    for(vector<string>::iterator it=visited.begin();it!=visited.end();it++)cout<<*it<<endl;
    cout<<'\n';
    return 0;
}
