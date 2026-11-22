#include <stdio.h>
#include <stdlib.h>
#define R 7
#define C 9
#define SZ (R*C)
int blocked[R][C]={
 {0,0,0,0,0,0,0,0,0},
 {0,1,1,1,0,1,1,1,0},
 {0,0,0,1,0,0,0,1,0},
 {1,1,0,1,1,1,0,1,0},
 {0,0,0,0,0,0,0,0,0},
 {0,1,1,1,1,1,1,1,0},
 {0,0,0,0,0,0,0,0,0}
};
int heuristic(int a,int b){int ar=a/C,ac=a%C,br=b/C,bc=b%C;return abs(ar-br)+abs(ac-bc);}
int search(int astar){
 int g[SZ],closed[SZ]={0},parent[SZ],explored=0,start=0,goal=R*C-1;
 for(int i=0;i<SZ;i++){g[i]=99999;parent[i]=-1;}
 g[start]=0;
 while(1){
  int u=-1,best=99999;
  for(int i=0;i<SZ;i++)if(!closed[i]&&g[i]<99999){
   int f=g[i]+(astar?heuristic(i,goal):0);
   if(f<best){best=f;u=i;}
  }
  if(u==-1)return -1;
  if(u==goal)return explored+1;
  closed[u]=1;explored++;
  int dr[4]={-1,1,0,0},dc[4]={0,0,-1,1};
  for(int z=0;z<4;z++){
   int r=u/C+dr[z],c=u%C+dc[z];
   if(r<0||r>=R||c<0||c>=C||blocked[r][c])continue;
   int v=r*C+c;
   if(!closed[v]&&g[u]+1<g[v])g[v]=g[u]+1,parent[v]=u;
  }
 }
}
int main(void){
 printf("Dijkstra explored: %d\n",search(0));
 printf("A* explored: %d\n",search(1));
 return 0;
}
