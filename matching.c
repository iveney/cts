#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//total is the maximum cardinality,p[1..n] means a match:i<->p[i]
#define maxn 100
int g[maxn][maxn],p[maxn],l[maxn][3],n,total,status[maxn],visited[maxn];
void solve() {
	int i,j,k,pass;
	memset(p,0,sizeof(p));
	do{
		i=0;
		do{
			if(p[++i])pass=0;
			else{
				memset(l,0,sizeof(l));
				l[i][2]=0xff;pass=path(i);
				for(j=1;j<=n;j++)for(k=1;k<=n;k++)
					if(g[j][k]<0)
						g[j][k] = -g[j][k];
			};
		}while(i!=n&&!pass);
		if(pass)total+=2;
	}while(i!=n&&total!=n);
}

void upgrade(int r) {
	int j=r,i=l[r][1];
	for(p[i]=j;l[i][2]<0xff;){
		p[j]=i;
		j=l[i][2];
		i=l[j][1];
		p[i]=j;
	}p[j]=i;
}


int path(int r){
	int i,j,k,v,t,quit;
	memset(status,0,sizeof(status));status[r]=2;
	do{quit=1;
		for(i=1;i<=n;i++)
			if(status[i]>1)
				for(j=1;j<=n;j++)
					if(g[i][j]>0&&p[j]!=i)
						if(status[j]==0){
							if(p[j]==0){
								l[j][1]=i;
								upgrade(j);
								return 1;}
							else if(p[j]>0){
								g[i][j]=g[j][i]=-1;status[j]=1;
								l[j][1]=i;g[j][p[j]]=g[p[j]][j]=-1;
								l[p[j]][2]=j;status[p[j]]=2;
								quit=0;
							}
						}else
							if(status[j]>1&&(status[i]+status[j]<6)){quit=0;g[i][j]=g[j][i]=-1;
								memset(visited,0,sizeof(visited));
								visited[i]=1;k=i;v=2;
								while(l[k][v]!=0xff){k=l[k][v];v=3-v;visited[k]=1;}k=j;v=2;
								while(!visited[k]){k=l[k][v];v=3-v;}if(status[i]!=3)l[i][1]=j;
								if(status[j]!=3)l[j][1]=i;
								status[i]=status[j]=3;t=i;v=2;
								while(t!=k){if(status[l[t][v]]!=3)l[l[t][v]][v]=t;
									t=l[t][v];status[t]=3;v=3-v;
								}
								t =j;v=2;
								while(t!=k){if(status[l[t][v]]!=3)l[l[t][v]][v]=t;
									t=l[t][v];status[t]=3;v=3-v;
								}
							}
	}while(!quit);
	return 0;
}

int main(){
	g[1][2]=50;
	g[2][3]=40;
	g[3][4]=30;
	g[4][5]=20;
	g[5][1]=10;
	n=5;
	solve();
	printf("MAX=%d\n",total);
	int i;
	for(i=1;i<=n;i++){
		printf("%d : %d\n",i,p[i]);
	}
	return 0;
}
