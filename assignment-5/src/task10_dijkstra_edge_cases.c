#include <stdio.h>
#include <limits.h>
#define N 4
#define INF LLONG_MAX
int main(void) {
    /* Case 1: vertex 3 is unreachable from 0. */
    int disconnected[N][N]={{0,5,0,0},{0,0,2,0},{0,0,0,0},{0,0,0,0}};
    long long d[N]={0,INF,INF,INF}; int used[N]={0};
    for(int k=0;k<N;k++){
        int u=-1;
        for(int i=0;i<N;i++)if(!used[i]&&d[i]!=INF&&(u==-1||d[i]<d[u]))u=i;
        if(u==-1) break;
        used[u]=1;
        for(int v=0;v<N;v++)if(disconnected[u][v]>0&&d[u]<=LLONG_MAX-disconnected[u][v]&&d[u]+disconnected[u][v]<d[v])
            d[v]=d[u]+disconnected[u][v];
    }
    printf("Unreachable test: vertex 3 = %s\n",d[3]==INF?"unreachable":"reachable");

    /* Case 2: negative edge. Dijkstra settles vertex 1 too early. */
    int neg[3][3]={{0,2,5},{0,0,0},{0,-10,0}};
    int settled[3]={0}; long long nd[3]={0,INF,INF};
    for(int k=0;k<3;k++){
        int u=-1;
        for(int i=0;i<3;i++)if(!settled[i]&&nd[i]!=INF&&(u==-1||nd[i]<nd[u]))u=i;
        if(u==-1) break;
        settled[u]=1;
        for(int v=0;v<3;v++)if(neg[u][v]!=0&&!settled[v]&&nd[u]+neg[u][v]<nd[v])nd[v]=nd[u]+neg[u][v];
    }
    printf("Negative-edge Dijkstra result to 1: %lld\n",nd[1]);
    printf("Correct path 0->2->1 has cost: -5 (Dijkstra assumption violated)\n");

    /* Case 3: safe large-distance addition using long long and a guard. */
    long long big=LLONG_MAX-100, weight=200;
    if(big<=LLONG_MAX-weight) puts("Large sum safe to add");
    else puts("Large sum would overflow; skip addition");
    return 0;
}
