#include <stdio.h>
#include <stdlib.h>
#include "testhelp.h"

int main(int argc, char **argv){
    test_cond("Check if 1 == 1", 1==1);
    test_cond("Check if 5 > 10", 5 > 10);
    test_report();

    return 0;
}
