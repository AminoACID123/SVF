#include <stdio.h>

int main()
{
    FILE *fp = fopen("out.txt", "w");
    fclose(fp);
    return 0;
}
