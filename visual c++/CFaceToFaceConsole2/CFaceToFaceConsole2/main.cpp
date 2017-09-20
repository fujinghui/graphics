#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void sort_bullet(int data[], int len){
	for (int i = 1; i < len; i++)
	{
		for (int j = i; j < len && data[j] < data[j-1]; j--)
		{
			int t = data[j];
			data[j] = data[j - 1];
			data[j - 1] = t;
		}
	}
}
void sort_insert(int data[], int len){
	int i, j;
	for (i = 1; i < len; i++)
	{
		j = i - 1;
		int k = data[i];
		while (j >= 0 && k < data[j])
		{
			data[j + 1] = data[j];
			j--;
		}
		data[j + 1] = k;
	}
}
void qksort(int data[], int i, int j){
	int si = i + 1, sj = j;
	int key = data[i];
	int position, temp;
	if (i >= j)
		return;
	else
	{
		while (si < sj)
		{

			//printf("(%d, %d)->", si, sj);
			while (data[si] <= key && si < sj)
			{
				//printf("-");
				si++;
			}
			while (data[sj] >= key && si < sj)
			{
				sj--;
			}
			printf("(%d, %d)\n", data[si], data[sj]);
			temp = data[si];
			data[si] = data[sj];
			data[sj] = temp;
		}

		if (data[si] > data[i])
		{
			int t = data[si - 1];
			data[si - 1] = data[i];
			data[i] = t;
			position = si - 1;		//要分割的位置
		}
		else
		{
			temp = data[si];
			data[si] = data[i];
			data[i] = temp;
			position = si;			//要分割的位置
		}
		qksort(data, i, position - 1);
		qksort(data, position + 1, j);
	}
}
void sort_quick(int data[], int start, int end){
	int t = start;
	int ss = start + 1;
	int ee = end;
	if (start >= end)
		return;
	while (ss < ee)
	{
		while (ss < ee && data[ss] <= data[start])
			ss++;
		while (ss < ee && data[ee] >= data[start])
			ee--;
		int tt = data[ss];
		data[ss] = data[ee];
		data[ee] = tt;
	}
	if (data[ss] > data[start])
	{
		int tt = data[ss-1];
		data[ss-1] = data[start];
		data[start] = tt;
		t = ss-1;
	}
	else
	{
		int tt = data[ss];
		data[ss] = data[start];
		data[start] = tt;
		t = ss;
	}
	sort_quick(data, start, t - 1);
	sort_quick(data, t + 1, end);
}
int count = 0;
int search_dich(int data[], int s, int e, int d){
	int m = (s + e) / 2;
	count++;
	if (s >= e)
		return 0;
	if (data[m] == d)
		return m;
	if (data[m] > d)
		return search_dich(data, s, m-1, d);
	if (data[m] < d)
		return search_dich(data, m+1, e, d);
}

int main(void){
	int data[100] = { 10, 32, 43, 54, 23, 43, 54, 65 };
	for (int i = 0; i < 100; i++)
		data[i] = rand() % 1000;

	sort_quick(data, 0, sizeof(data) / sizeof(int) - 1);
	for (int i = 0; i < sizeof(data) / sizeof(int); i++)
	{
		printf("%d ", data[i]);
		if ((i+1) % 10 == 0)
			printf("\n");
	}
	printf("find :%d count:%d\n", search_dich(data, 0, sizeof(data) / sizeof(int), 805), count);
	return 0;
}
