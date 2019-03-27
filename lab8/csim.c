//name: Hou Zhengtong
//loginID: ics517030910183
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char *argv[])
{
    int index = 0;
    int s,E,b;
    char ch
    while((ch = getopt(argc,argv,"s:E:b:")) != EOF)
    {
        switch(ch)
        {
            case 's':
                s = optarg - '0';
                break;
            case 'E':
                E = optarg - '0';
                break;
            case 'b':
                b = optarg - '0';
                break;
            default:
                return 1; 
        }
    }
    argc -= optind;
    argv -= optind;
    for(index = 0; index < argc; index++)
    {
        puts(argv[index]);
    }
    printf("s:%x",s);
    printf("E:%x",E);
    printf("b:%x",b);
    printSummary(0, 0, 0);
    return 0;
}
