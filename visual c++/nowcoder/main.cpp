#include <iostream>


int main(int argc, char** argv) {
	int circle[100001];
	//int visited[100001][100001];		//�������λ���Ƿ���ܿ��� 
	int i, n;
	int count = 0;
	scanf("%d", &n);
	for(i = 0; i < n; i ++)
	{
		scanf("%d", &circle[i]);
	}
	count = n;
	int s = n / 2;//(n % 2 == 0)?(n/2):(n/2+1);
	//�ӵ�һ��6�㵽һ��ĵ㿪ʼ���� 
	for(int point = 0; point < s; point ++)
	{
		
		int to, en;
		for(to = point + 2;to < point + n; to ++)
		{
			if(circle[to%n] >= circle[(to-1)%n] && circle[point] >= circle[(to-1)%n])
			{
				count ++;
				printf("forward:%d ", circle[to%n]); 
			} 
			else
				break;
		}
		
		for(en = point - 2; (en+n)%n > to; en -= 1)
		{
			if(circle[(en+n)%n] >= circle[(en+1+n)%n] && circle[point] >= circle[(en+1+n)%n])
			{
				count ++;
				printf("backward:%d ", circle[(en+n)%n]);
			}
			else
				break;
		}
		printf("\n");
	}
	printf("%d\n", count);
	
	return 0;
}
