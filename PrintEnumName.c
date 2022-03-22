#include <stdio.h>

#define MY_INT 7777
#define MY_STRING "9999"
#define MY_STRING2 "9999\"8OK"000(888)
#define ENUM_CHIP_TYPE_CASE(x)   case x: return(#x);
#define MY_DEFINE(R) #R
#define MY_DEFINE2(R) MY_DEFINE(R)

enum cvmx_chip_types_enum {
    CVMX_CHIP_TYPE_NULL = 0,
    CVMX_CHIP_TYPE_DEPRECATED = 1,
    CVMX_CHIP_TYPE_OCTEON_SAMPLE = 2,
    CVMX_CHIP_TYPE_MAX
};


static inline const char *cvmx_chip_type_to_string(enum cvmx_chip_types_enum type)
{
    switch (type)
    {
        ENUM_CHIP_TYPE_CASE(CVMX_CHIP_TYPE_NULL)
            ENUM_CHIP_TYPE_CASE(CVMX_CHIP_TYPE_DEPRECATED)
            ENUM_CHIP_TYPE_CASE(CVMX_CHIP_TYPE_OCTEON_SAMPLE)
            ENUM_CHIP_TYPE_CASE(CVMX_CHIP_TYPE_MAX)
    }
    return "Unsupported Chip";

}

int main()
{
    enum cvmx_chip_types_enum a;
    int my_int_test = 7;

    printf("1->%s\r\n", MY_DEFINE(CVMX_CHIP_TYPE_DEPRECATED));
    printf("2->%s\r\n", MY_DEFINE(my_int_test));
    printf("3->%s\r\n", MY_DEFINE(8888));
    printf("4->%s\r\n", MY_DEFINE(MY_INT));
    printf("5->%s\r\n", MY_DEFINE2(MY_INT));
    printf("6->%s\r\n", "MY_INT");
    printf("7->%s\r\n", MY_DEFINE(MY_STRING));
    printf("8->%s\r\n", MY_DEFINE2(MY_STRING));
    printf("9->%s\r\n", MY_DEFINE(MY_STRING2));
    printf("10->%s\r\n", MY_DEFINE2(MY_STRING2));

    while(1)
    {
        printf("input(0~3):");
        scanf("%d",&a);
        if(a<CVMX_CHIP_TYPE_NULL||a>CVMX_CHIP_TYPE_MAX)
        {
            printf("error,you must input number between 0 and 3,please try again!\n\n");
            continue;
        }
        printf("\n");

        printf("the result:%s \n\n",cvmx_chip_type_to_string(a));
    }

    return 0;
}