#include <iostream>
#include <string.h>
using namespace std;
int main(void){
	char buffer[1001];
	int data[10];
	memset(data, 0, sizeof(data));
	cin>>buffer;
	for(int i = 0; i < strlen(buffer); i ++)
	{
		data[buffer[i]-'0'] ++;
	}
	for(int i = 0;i < 10;  i++)
	{
		if(data[i] != 0)
		{
			printf("%d:%d\n", i, data[i]);
		}
	}
	return 0;
}
