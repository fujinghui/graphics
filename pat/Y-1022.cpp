#include <iostream>
using namespace std;
void print(int m, int k){
	if(m/k == 0)
		cout<<m;
	else
	{
		print(m/k, k);
		cout<<(m%k);
	}
}
int main(void){
	int a, b, d, s;
	int buffer[20];
	int len = 0;
	a = 64;
	b = 0;
	d = 8;
	cin>>a>>b>>d;
	print(a+b, d);
	return 0;
}
