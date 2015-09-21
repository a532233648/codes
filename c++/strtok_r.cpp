/*************************************************************************
	> File Name: strtok_r.cpp
	> Author: zhouzhibo
	> Mail: 532233648@qq.com 
	> Created Time: 2015年07月31日 星期五 15时18分18秒
 ************************************************************************/

#include<iostream>
#include<string.h>
#include<stdio.h>
using namespace std;
int LastPos(char s[], char ch) {
    int i,pos = 0; //
    for(i = 0; s[i]; ++i)
        if(s[i] == ch) pos = i + 1;
    return pos;
}
 
// 复制s[]中右边的n个字符到t[]中
char *RightStr(char s[], char t[], int n) {
    int i,j,len = strlen(s);
    t[0] = '\0';
    if(n >= len) strcpy(t,s);
    else if(n > 0 && n < len) {
        i = len - n;
        j = 0;
        while(t[j++] = s[i++])
            ;
    }
    return t;
}
 
int main() {
    char s[] = "C:<a href=https://www.baidu.com/s?wd=WINDOWS&tn=44039180_cpr&fenlei=mv6quAkxTZn0IZRqIHckPjm4nH00T1Y1my79uym4PjnvuW9WnARz0ZwV5Hcvrjm3rH6sPfKWUMw85HfYnjn4nH6sgvPsT6K1TL0qnfK1TL0z5HD0IgF_5y9YIZ0lQzqlpA-bmyt8mh7GuZR8mvqVQL7dugPYpyq8Q1TYPjfLPjnsP1mYrHRYPjn1n6target=>WINDOWS</a>\\|123";
    char t[20];
    printf("'tt'在s[]中最后出现的位置是:%d\n",LastPos(s,'|'));
    printf("t[] = %s\n",RightStr(s,t,3));
    return 0;
}
