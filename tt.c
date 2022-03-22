
#include <stdio.h>
#include "format.h"

int main(){

    printf("1");
    struct frameData frame;
    printf("2");
    struct frameData * f = &frame;
    printf("3");
    char *mp4_avc1 = "../../formatVideo/2.mp4";
    int aa = extract(mp4_avc1,f);
    printf("%d\n",aa);
    printf("%d\n",f->width);
    printf("%d\n",f->height);
}