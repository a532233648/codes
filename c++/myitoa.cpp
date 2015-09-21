/*************************************************************************
	> File Name: temp.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月22日 星期三 15时34分20秒
 ************************************************************************/

#include<iostream>
#include<string.h>
using namespace std;
#include <stdio.h>
#include<stdlib.h>

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
int main()
{
        char string[]="201";
        char ip_string[1024];
        int ip_num=10;
        int ip_1;
        myitoa(ip_num, ip_string);
        ip_1=atoi(string);
        cout<<ip_num<<"\n"<<ip_string<<endl;
        cout<<ip_1<<"\n"<<string<<endl;
        return 0;
}
