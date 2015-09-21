/*************************************************************************
	> File Name: temp.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月24日 星期五 11时31分52秒
 ************************************************************************/

#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;



int DividIp(char divid_ip[],char first_ip[],char last_ip[]){
    int divid_loc,i,j;
    for(i=0;i<strlen(divid_ip);i++){
        //if(strcmp(divid_ip[i],'-')==0&&strcmp(divid_ip[i-1]," ")==0&&strcmp(divid_ip[i+1]," ")==0)divid_loc=i;
        if(divid_ip[i]=='-'&&divid_ip[i-1]==' ' &&divid_ip[i+1]==' ' )divid_loc=i;
    }
    strncpy(first_ip,divid_ip,divid_loc-1);
    for(i=divid_loc+2,j=0;i<strlen(divid_ip),j<strlen(divid_ip)-divid_loc+1;i++,j++){
        last_ip[j]=divid_ip[i];
    }
    return 0;
    
}
int main(){
    char divid_ip[]="204.0.0.0 - 204.255.255.255";
    char fi[1024],la[1024];
    DividIp(divid_ip,fi,la);
    cout<<fi<<"\n"<<la<<endl;
}



