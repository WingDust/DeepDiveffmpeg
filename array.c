#include <stdio.h>
#include <stdint.h>

#define N 5

// 反转输入的5个数
void reverse_5(){
    int a[N];
    int i;

    printf("Enter %d numbers:",N);
    for ( i = 0; i < N; i++)
    {
        scanf("%d",&a[i]);
    }
    printf("in reverse order:");
    for (i =N- 1; i >= 0; i--)
    {
        printf("%d ",a[i]);
    }
        printf("\n");
}

// 输出输入的3个数
void scanf3(){
    int a,b,c;

    printf("请输入三个数字");
    scanf("%d%d%d",&a,&b,&c);
    printf("%d,%d,%d\n",a,b,c);
}

void writeyuv(){
    char *yuv = "./out.yuv";
    char *y = "\205";
    char *u = "h";
    char *v = "\230";
    FILE *f = fopen(yuv,"wb+");
    // int y = 27;
    // int u = 128;
    // int v = 128;
    // fwrite(y,1,604*1080,f);
    // fwrite(u,1,604*1080/4,f);
    // fwrite(v,1,604*1080/4,f);
}

void uint(){
    // uint8_t *d[3] = {"1","2","3"};
    // uint8_t d1 = "23";
    // uint8_t d2 = "2";
    // uint8_t d3 = "255";
    // printf("%s",d);
}

void fw(){
    // char *demo = "out.txt";
    char *demo = "out";
    FILE *f = fopen(demo,"wb+");
    fwrite(demo,1,/*7*/10,f);
    fclose(f);
}
void fr(){
    // char *demo  = "out.txt";
    char *demo = "out";
    FILE *f = fopen(demo,"rb+");
    char ctx[10];
    int numread = fread(ctx,1,/*7*/10,f);
    printf("%d",numread);
    printf("%s",ctx);
    fclose(f);
}

int main (void){
   fr();
   return 0;
}