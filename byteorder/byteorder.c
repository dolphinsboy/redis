#include <stdio.h>
#include <stdlib.h>


int main(){
    union {
        short int a;
        unsigned char c[2];
    }s;

    s.a = 0x0102;

    if(s.c[0] == 1 && s.c[1] == 2){
        printf("big end\n");
    }else if(s.c[0] == 2 && s.c[1] == 1){
        printf("little end\n");
    }else{
        printf("unknow");
    }

    return 0;

}
