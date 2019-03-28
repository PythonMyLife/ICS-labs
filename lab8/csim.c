//name: Hou Zhengtong
//loginID: ics517030910183
#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int hits = 0;
int misses = 0;
int evictions = 0;
typedef struct{
    int Lru;
    int valid;
    int tag;
} Line;

typedef struct{
    Line *lines;
} Set;

typedef struct{
    int set_number;
    int line_number;
    Set *sets;
} Cache;

int simcache(int s,int E,int b,Cache *cache)
{
    if(s < 0 ) return 0;
    cache->set_number = 2 << s;
    cache->line_number = E;
    if(!(cache->sets = (Set *)malloc(cache->set_number * sizeof(Set))))
        return 0;
    for(int i = 0; i < cache->set_number; i++){
        if(!(cache->sets[i].lines = (Line *)malloc(E * sizeof(Line))))
            return 0;
        for(int j = 0; j < E; j++){
            cache->sets[i].lines[j].valid = 0;
            cache->sets[i].lines[j].Lru = 0;
        }
    }
    return 1;
}

int main(int argc,char *argv[])
{
    int flag = 0;
    //int index = 0;
    int s,E,b,v = 0;
    char filename[100];
    Cache cache;
    int o;
    while((o = getopt(argc,argv,"vs:E:b:t:")) != -1)
    {
        switch(o)
        {
            case 'v':
                v = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(filename,optarg);
                break;
            default:
                exit(0); 
        }
    }
    printf("%s\n",filename);
    printf("s:%x\n",s);
    printf("E:%x\n",E);
    printf("b:%x\n",b);
    if(!(flag = simcache(s,E,b,&cache)))
        exit(0);
    FILE *trace = fopen(filename,"r");

    printSummary(0, 0, 0);
    return 0;
}
