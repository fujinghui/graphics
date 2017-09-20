#include <iostream>
#include <algorithm>
#include <vector>
#include <string.h>
#include <windows.h>
#include <gl/glu.h>

using namespace std;
int main(void){
	char buffer[10000] = "-11111E-02";
	char digit[10000];
	int edit = 0;
	int len1 = 0, len2 = 0, len3 = 0;
	int flag1 = 0, flag2 = 0;
	int point_index = 0;
	int i;
	cin>>buffer;
	len3 = strlen(buffer);
	i = 0;

	if(buffer[0] == '-' || buffer[0] == '+')
	{
		i ++;
	}
	if(buffer[0] == '-')
		flag1 = 1;
	while((buffer[i] >= '0' && buffer[i] <= '9')||buffer[i] == '.')
	{
		//记住小数点的位置
		if(buffer[i] == '.')
			point_index = len1;
		digit[len1 ++] = buffer[i ++];
	}
	digit[len1] = 0;

	if(buffer[i] == 'E')
		i ++;
	if(buffer[i] == '-')
	{
		i ++;
		flag2 = 1;
	}
	else if(buffer[i] == '+')
	{
		i ++;
	}
	while(buffer[i] >= '0' && buffer[i] <= '9')
	{
		edit = edit * 10 + buffer[i]-'0';
		i ++;
	}
	if(flag1 == 1)
		cout<<'-';



	if(flag2 == 1)
	{
		if(point_index == 0)
		{
			point_index = len1;
		}

		{
			if(edit >= (point_index))
			{
				cout<<'0';
				cout<<".";
				for(int i = 1; i <= edit-point_index; i ++)
					cout<<'0';
				for(int i = 0; i < len1; i ++)
				{
					if(digit[i] != '.')
						cout<<digit[i];
				}
			}
			else
			{
				int t = 0;
				for(int i = 0; i < point_index-edit; i ++)
				{
					cout<<digit[i];
				}
				cout<<'.';
				for(int i = point_index - edit;i < len1; i ++)
				{
					if(digit[i] != '.')
						cout<<digit[i];
				}
			}
		}
	}
	else
	{
		//向前
		int i = 0;
		while(digit[i] != '.' &&i < len1)
			cout<<digit[i++];

		//有小数点
		if(i < len1)
		{
			i ++;
			if(len1 - point_index > edit)
			{
				for(int j = 0; j < edit; j ++)
				{
					cout<<digit[i];
					i ++;
				}
				cout<<'.';
				for(; i < len1; i ++)
				{
					cout<<digit[i];
				}
			}
			else
			{
				for(; i < len1; i ++)
				{
					cout<<digit[i];
				}
				for(int j = 0; j <= edit-len1+point_index; j ++)
				{
					cout<<'0';
				}
			}
		}
		else		//没有小数点
		{
			for(int j = 0; j < edit; j ++)
			{
				cout<<'0';
			}
		}
		if(i < len1)
			i ++;
	}


/*
	cout<<endl;
	cout<<flag1<<":"<<digit;
	cout<<endl;
	cout<<flag2<<":"<<edit;
	cout<<endl;
*/
	return 0;
}
