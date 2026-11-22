#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 8
#define INF 1000000000
int main(void) {
    int g[N][N]={
        {0,4,2,0,0,0,0,0},{4,0,1,5,0,0,0,0},
        {2,1,0,8,10,0,0,0},{0,5,8,0,2,6,0,0},
        {0,0,10,2,0,3,5,0},{0,0,0,6,3,0,1,7},
        {0,0,0,0,5,1,0,2},{0,0,0,0,0,7,2,0}
    };
    int d[N],done[N]={0},fw[N][N];
    for(int i=0;i<N;i++){d[i]=INF;for(int j=0;j<N;j++)fw[i][j]=i==j?0:(g[i][j]?g[i][j]:INF);}
    d[0]=0;
    for(int k=0;k<N;k++){
        int u=-1;
        for(int i=0;i<N;i++)if(!done[i]&&(u==-1||d[i]<d[u]))u=i;
        done[u]=1;
        for(int v=0;v<N;v++)if(g[u][v]&&d[u]+g[u][v]<d[v])d[v]=d[u]+g[u][v];
    }
    for(int k=0;k<N;k++)for(int i=0;i<N;i++)for(int j=0;j<N;j++)
        if(fw[i][k]<INF&&fw[k][j]<INF&&fw[i][k]+fw[k][j]<fw[i][j])fw[i][j]=fw[i][k]+fw[k][j];
    puts("Source 0 distances (Dijkstra array):");
    for(int i=0;i<N;i++)printf("%d ",d[i]);
    puts("\nFloyd-Warshall row 0:");
    for(int i=0;i<N;i++)printf("%d ",fw[0][i]);
    puts("\nBoth methods should match.");
    return 0;
}
