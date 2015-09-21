/*************************************************************************
	> File Name: comp_ip.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月28日 星期二 13时39分13秒
 ************************************************************************/

#include<iostream>
#include<vector>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<algorithm>
#include<map>
using namespace std;
class DirCheck{
    private:
        map<int, string> visited_ip_map;
    public:
        int judge_ip(char ip[]);
        int comipqueue(char comp_ip[]);
        int comtwoip(char ip1[],char ip2[]);
        int DividOneip(char ip[],int ip_num[]);
        int DividIp(char divid_ip[],char first_ip[],char last_ip[]);

};



int DirCheck::DividIp(char divid_ip[],char first_ip[],char last_ip[]){
    int divid_loc,i,j;
    for(i=0;i<strlen(divid_ip);i++){
        //if(strcmp(divid_ip[i],'-')==0&&strcmp(divid_ip[i-1]," ")==0&&strcmp(divid_ip[i+1]," ")==0)divid_loc=i;
        if(divid_ip[i]=='-'&&divid_ip[i-1]==' ' &&divid_ip[i+1]==' ' ){
            divid_loc=i;
            break;
        }
    }
    strncpy(first_ip,divid_ip,divid_loc-1);
    for(i=divid_loc+2,j=0;i<strlen(divid_ip),j<strlen(divid_ip)-divid_loc+1;i++,j++){
        last_ip[j]=divid_ip[i];
    }
    return 0;
    
}
int DirCheck::judge_ip(char ip[]){
    int r1=strcmp(ip,"0.0.0.0");
    int r2=strcmp(ip,"255.255.255.255");
    if(r1==0)return 1;
    else if(r2==0)return 2;
    else return 0;
}


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
int DirCheck::comtwoip(char ip1[],char ip2[]){
    int ip1_num[5],ip2_num[5];
    if(strcmp(ip1,ip2)==0)return 0;
    DividOneip(ip1,ip1_num);
    DividOneip(ip2,ip2_num);
    //for(int i=0;i<4;i++)cout<<"[ip1_num]"<<ip1_num[i]<<'\t';
    //cout<<endl;
    //for(int i=0;i<4;i++)cout<<"[ip2_num]"<<ip2_num[i]<<'\t';
    //cout<<'\n';
    for(int i=0;i<4;i++){
        if(ip1_num[i]>ip2_num[i])return 1;
        else if(ip1_num[i]<ip2_num[i]) return 2;
    }
}

int DirCheck::comipqueue(char comp_ip[]){
    char ip[11][1024]={"0.0.0.255","25.0.0.255","50.0.0.255","75.0.0.255","100.0.0.255","125.0.0.255","150.0.0.255","175.0.0.255","200.0.0.255","225.0.0.255","250.0.0.255"};
    int flag=0,loc;
    int ip_num[5];
    char temp[1024],first_ip[1024],last_ip[1024];
    DividIp(comp_ip,first_ip,last_ip);
    for(int i=0;i<11;i++){
        if(comtwoip(first_ip,ip[i])==2&&comtwoip(last_ip,ip[i])==1)return 1;
    }
    return 0;
   

}

int main(){
    char ip1[]="";
    char ip2[]="";
    char ip[]="75.12.1.2";
    char ip1toip2[]="125.0.0.0 - 125.1.255.255 ";
    DirCheck dir;
    int i=dir.comipqueue(ip1toip2);
    cout<<i<<endl;
    return 0;
}
