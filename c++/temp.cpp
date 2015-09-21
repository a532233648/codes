/*************************************************************************
	> File Name: temp.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月24日 星期五 12时40分14秒
 ************************************************************************/

#include<iostream>
#include<string.h>
#include<queue>
#include<stdlib.h>
#include<stdio.h>
using namespace std;
queue<string>visiting_ip_queue,first_ip_queue,last_ip_queue;


int myitoa(int i, char *string)
{
        int power=0,j=0;

        j=i;
        for( power=1;j>=10;j/=10)
                power*=10;

        for(;power>0;power/=10)
        {
                *string++='0'+i/power;
                i%=power;
        }
        *string='\0';
        //printf("%s\n",string);
        return 0;
}

int DividOneip(char ip[],int ip_num[]){
    char tmp_ip[1024],temp[1024];
    strcpy(tmp_ip,ip);
    int k=0,j=0;
    for(int i=0;i<=strlen(tmp_ip);i++){
        cout << "in " << __func__ <<"K = " << k <<endl;
        temp[j]=tmp_ip[i];
        j++;
        if(tmp_ip[i]=='.'||tmp_ip[i]=='\0'){
            temp[j]='\0';
            ip_num[k]=atoi(temp);
            k++;
            j=0;
        }
    }
    return 0;

}

int AddQueue(char first_ip[],char last_ip[]){
    int ip_num[5];
    int j=3;
    char tp_ip[1024];
    char str_1[5],str_2[5],str_3[5],str_4[5],str[]=".";
    char *temp_ip;
    int r1=strcmp(first_ip,"0.0.0.0");
    int r2=strcmp(last_ip,"255.255.255.255");
    if(r1==0&&r2==0)return 0;
    if(r1!=0&&r2==0)return 0;
    first_ip_queue.push(first_ip);
    last_ip_queue.push(last_ip);
    strcpy(tp_ip,last_ip);
    DividOneip(tp_ip,ip_num);
    for(int sa=0;sa<4;sa++)cout<<"[de:]"<<ip_num[sa]<<"\t";
    cout<<"\n";
    ip_num[j]++;
    char str_one[5],str_two[5],str_thr[5],str_four[5];
    while(j>=0){
        if(ip_num[j]>255&&j>0){
            ip_num[j]=ip_num[j]-256;
            ip_num[j-1]=ip_num[j-1]+1;
        }
        j--;
    }
    myitoa(ip_num[0],str_1);
    myitoa(ip_num[1],str_2);
    myitoa(ip_num[2],str_3);
    myitoa(ip_num[3],str_4);
    for(int sa=0;sa<4;sa++)cout<<"[lde:]"<<ip_num[sa]<<"\t";
    cout<<"\n";
    cout<<"[strde]"<<str_1<<"\t"<<str_2<<"\t"<<str_3<<"\t"<<str_4<<"\n";
    temp_ip=new char[1024];
    myitoa(ip_num[0],str_1);
    strcat(temp_ip,str_1);
    strcat(temp_ip,".");
    myitoa(ip_num[1],str_2);
    strcat(temp_ip,str_2);
    strcat(temp_ip,".");
    myitoa(ip_num[2],str_3);
    strcat(temp_ip,str_3);
    strcat(temp_ip,".");
    myitoa(ip_num[3],str_4);
    strcat(temp_ip,str_4);
    cout<<"[Debug visiting ip:]"<<temp_ip<<endl;
    visiting_ip_queue.push(temp_ip);
    memset(temp_ip,'\0',sizeof(temp_ip));
    delete []temp_ip;
    return 0;
}

int main(){
    char first_ip[]="1.5.69.112";
    char last_ip[]="255.10.1.255";
    while(1){
    cin>>last_ip;
    AddQueue(first_ip,last_ip);
    }
    
    return 0;
}
