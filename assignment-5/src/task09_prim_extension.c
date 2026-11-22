#include <stdio.h>
#include <limits.h>
#define V 6
void prim(int c[V][V]){
 int key[V],par[V],in[V]={0},total=0;
 for(int i=0;i<V;i++)key[i]=INT_MAX,par[i]=-1;
 key[0]=0;
 for(int k=0;k<V;k++){
  int u=-1;
  for(int i=0;i<V;i++)if(!in[i]&&(u==-1||key[i]<key[u]))u=i;
  if(u==-1||key[u]==INT_MAX){puts("Network disconnected");return;}
  in[u]=1;total+=key[u];
  for(int v=0;v<V;v++)if(c[u][v]>0&&!in[v]&&c[u][v]<key[v])key[v]=c[u][v],par[v]=u;
 }
 for(int v=1;v<V;v++)printf("%d-%d cost %d\n",par[v],v,c[par[v]][v]);
 printf("Total cable cost = %d\n",total);
}
int main(void){
 int c[V][V]={
 {0,4,0,6,0,0},{4,0,3,5,7,0},{0,3,0,0,4,6},
 {6,5,0,0,2,0},{0,7,4,2,0,3},{0,0,6,0,3,0}
 };
 puts("Original cable plan:");prim(c);
 c[1][2]=c[2][1]=0;
 puts("\nAfter cable 1-2 is unavailable:");prim(c);
 return 0;
}
