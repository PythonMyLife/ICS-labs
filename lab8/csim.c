//name: Hou Zhengtong
//loginID: ics517030910183
#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LRU 1023;
#define GETSET(addr, s, b) ((addr >> b) & ((1 << s) - 1))
#define GETTAG(addr, s, b) (addr >> (s + b))


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

int findLru(Cache *cache,int set_bits);
void updateLru(Cache *cache, int set_bits, int index);
int miss(Cache *cache, int set_bits, int tag_bits);
int eviction(Cache *cache, int set_bits,int tag_bits);
void load(Cache *cache,int addr, int size,int set_bits,int tag_bits,int v);

/* simulate cache */
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
    int s,E,b,v = 0;
    char filename[100],opt[10];
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
    if(!(flag = simcache(s,E,b,&cache)))
        exit(0);
    FILE *trace = fopen(filename,"r");

    int addr,size;
    while(fscanf(trace,"%s %x,%d",opt,&addr,&size) != EOF){
        if(strcmp(opt,"I") == 0) continue;
        int set_bits = GETSET(addr,s,b);
        int tag_bits = GETTAG(addr,s,b);
        if(v == 1) printf("%s %x,%d", opt, addr, size);
        if(strcmp(opt, "S") == 0){
            load(&cache,addr,size,set_bits,tag_bits,v);
        }
        if(strcmp(opt, "L") == 0){
            load(&cache,addr,size,set_bits,tag_bits,v);
        }
        if(strcmp(opt, "M") == 0){
            load(&cache,addr,size,set_bits,tag_bits,v);
            load(&cache,addr,size,set_bits,tag_bits,v);
        }
        if(v == 1) printf("\n");
    }

    printSummary(hits, misses, evictions);
    return 0;
}

/* find the smallest Lru index */
int findLru(Cache *cache,int set_bits)
{
    int index = 0;
    int min = MAX_LRU;
    for(int i = 0; i < cache->line_number; i++){
        if(cache->sets[set_bits].lines[i].Lru < min){
            index = i;
            min = cache->sets[set_bits].lines[i].Lru;
        }
    }
    return index;
}

/* update Lru */
void updateLru(Cache *cache, int set_bits, int index)
{
    cache->sets[set_bits].lines[index].Lru = MAX_LRU;
    for(int j = 0; j < cache->line_number; j++){
        if(j != index) cache->sets[set_bits].lines[j].Lru--;
    }
}

/* judge miss or not */
int miss(Cache *cache, int set_bits, int tag_bits)
{
    int i,flag = 1;
    for(i = 0; i < cache->line_number;i++){
        if(cache->sets[set_bits].lines[i].valid == 1 && cache->sets[set_bits].lines[i].tag == tag_bits){
            flag = 0;
            updateLru(cache,set_bits,i);
        }
    }
    return flag;
}

/* judge eviction or not */
int eviction(Cache *cache, int set_bits,int tag_bits)
{
    int i,flag = 1;
    for(i = 0; i < cache->line_number; i++){
        if(cache->sets[set_bits].lines[i].valid == 0){
            flag = 0;
            break;
        }
    }
    if(flag == 0){
        cache->sets[set_bits].lines[i].valid = 1;
        cache->sets[set_bits].lines[i].tag = tag_bits;
        updateLru(cache, set_bits, i);
    }
    else{
        int index = findLru(cache,set_bits);
        cache->sets[set_bits].lines[index].valid = 1;
        cache->sets[set_bits].lines[index].tag = tag_bits;
        updateLru(cache, set_bits, index);
    }
    return flag;
}

/* store,load,and modify */
void load(Cache *cache, int addr, int size, int set_bits,int tag_bits,int v)
{
    if(miss(cache,set_bits,tag_bits) == 1){
        misses++;
        if(v == 1) printf(" miss");
        if(eviction(cache, set_bits, tag_bits) == 1){
            evictions++;
            if(v == 1) printf(" eviction");
        }
    }
    else{
        hits++;
        if(v == 1) printf(" hit");
    }
}
