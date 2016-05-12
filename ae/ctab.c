#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ae.h"

int a = 0;

int serverCron(){
    printf("serverCron %d\n", a);
    a += 1;

    sleep(5);

    return 0;

}

int main(int argc, char **argv){
    struct aeEventLoop* eventLoop = aeCreateEventLoop(1000);

    if(aeCreateTimeEvent(eventLoop, 1, serverCron, NULL, NULL) == AE_ERR) {
        printf("Can't create the serverCron time event.");
        exit(1);
    }

    aeMain(eventLoop);

    aeDeleteEventLoop(eventLoop);

    
}
