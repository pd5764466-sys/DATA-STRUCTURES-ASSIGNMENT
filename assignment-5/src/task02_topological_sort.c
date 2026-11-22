#include <stdio.h>
#define V 6
const char *task[V]={"Lease","Power","Racks","Network","Staff","Launch"};
int g[V][V]={
    {0,1,1,0,1,0},
    {0,0,0,1,0,0},
    {0,0,0,1,0,0},
    {0,0,0,0,0,1},
    {0,0,0,0,0,1},
    {0,0,0,0,0,0}
};
int main(void) {
    int indeg[V]={0}, q[V], head=0, tail=0, done=0;
    for(int u=0;u<V;u++)
        for(int v=0;v<V;v++) indeg[v]+=g[u][v];
    for(int v=0;v<V;v++) if(indeg[v]==0) q[tail++]=v;
    while(head<tail) {
        int u=q[head++];
        printf("%s ",task[u]);
        done++;
        for(int v=0;v<V;v++) if(g[u][v]) {
            indeg[v]--;
            if(indeg[v]==0) q[tail++]=v;
        }
    }
    printf("\n%s\n",done==V?"valid schedule":"cycle detected");
    return 0;
}
