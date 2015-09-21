/*************************************************************************
	> File Name: map.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月28日 星期二 15时23分44秒
 ************************************************************************/

#include<iostream>
#include <map>
#include <string>
using namespace std;
int main(){
     char ip[11][1024]={"0.0.0.255","25.0.0.255","50.0.0.255","75.0.0.255","100.0.0.255","125.0.0.255","150.0.0.255","175.0.0.255","200.0.0.255","225.0.0.255","250.0.0.255"};
      map<int, string> visited_ip_map;
     for(int i=0;i<11;i++){
         visited_ip_map.insert(pair<int, string>(i, ip[i]));
     }
     for(map<int, string>::iterator it = visited_ip_map.begin(); it != visited_ip_map.end(); it++)
    {
        cout<<it->first<<'\t'<<it->second<<endl;
    }
    return 0;
}
