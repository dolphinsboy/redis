#include <stdio.h>
#include <stdlib.h>

int IsBigEndian()
{
    int a =1 ; 
    if(((char*)&a)[3] ==1)
        return 1;
    else
        return 0;
}

int main(void)
{
  int bool = IsBigEndian();

  if(bool == 1)
      printf("Big\n");
  else
      printf("Low\n");
}
