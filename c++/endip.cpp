/*************************************************************************
	> File Name: endip.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月29日 星期三 11时10分55秒
 ************************************************************************/

#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;
class DirCheck{
    private:
    public:
        int DividOneip(char ip[],int ip_num[]);
        int ipend(char end_ip[]);
};

int DirCheck::DividOneip(char ip[],int ip_num[]){
    char tmp_ip[1024],temp[1024];
    strcpy(tmp_ip,ip);
    int k=0,j=0;
    for(int i=0;i<=strlen(tmp_ip);i++){
        //cout<<"in "<<__func__ << k <<endl;
        temp[j]=tmp_ip[i];
        j++;
        if(tmp_ip[i]=='.'||tmp_ip[i]=='\0'){
            temp[j]='\0';
            ip_num[k]=atoi(temp);
            k++;
            j=0;
        }
    }
    k=0;
    return 0;

}
int DirCheck::ipend(char end_ip[]){
    int ip_num[5];
    DividOneip(end_ip,ip_num);
    if(ip_num[0]==255&&ip_num[1]==255&&ip_num[2]>255)return 1;
    else return 0;
}

int main(){
    char ip[]="255.255.256.0";
    DirCheck dc;
    if(dc.ipend(ip)==1)cout<<"end:"<<endl;
    else cout<<"not end"<<endl;
    return 0;
}
