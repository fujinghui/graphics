#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

int main(void){
	int N;
	long long p;
	long long data[100001];
	int max_value = 0;
	cin>>N>>p;
	for(int i = 0; i < N; i ++)
	{
		cin>>data[i];
		/*int j = i - 1;
		long long k = data[i];
		while(j >= 0 && data[j] > k)
		{
			data[j+1] = data[j];
			j --;
		}
		data[j+1] = k;*/
	}
	sort(data, data+N);
	for(int i = 0; i < N && N > max_value + i; i ++)
	{
		for(int j = i + 1 + max_value; j < N; j ++)
		{
			if(data[j] <= data[i] * p)
			{
				if((j - i)>max_value)
				{
					max_value = j - i;
				}
			}
		}
	}
	cout<<(max_value+1);
	return 0;
}
