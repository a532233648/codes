/*************************************************************************
	> File Name: temp.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月23日 星期四 15时25分15秒
 ************************************************************************/

#include<iostream>
#include<string>
#include<stdlib.h>
#include<string.h>
using namespace std;
int DividOneip(char ip[],int ip_num[]){
    char tmp_ip[1024],temp[1024];
    strcat(tmp_ip,ip);
    //cout<<tmp_ip<<endl;
    int k,j=0;
    for(int i=0;i<=strlen(tmp_ip);i++){
        temp[j]=tmp_ip[i];
        //cout<<temp[j]<<endl;
        j++;
        if(tmp_ip[i]=='.'||tmp_ip[i]=='\0'){
            //temp[j]='\0';
            //cout<<temp<<endl;
            ip_num[k]=atoi(temp);
            //cout<<ip_num[k]<<endl;
            k++;
            j=0;
        }
    }
    return 0;
}

int main(){
    char ip[1024]="123.321.108.81";
    int ip_num[5];
    DividOneip(ip,ip_num);
    for(int i=0;i<4;i++){
        cout<<ip_num[i];
        cout<<"\n";
    }
    return 0;
}
