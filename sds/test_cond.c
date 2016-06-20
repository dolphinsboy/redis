#include <stdio.h>
#include <stdlib.h>
#include "testhelp.h"

int main(int argc, char**argv){
    test_cond("test 2>1", 2>1);
    test_cond("test foo==foo", "foo"=="foo");
    test_cond("test 1>2", 1>2);

    test_report();
}
