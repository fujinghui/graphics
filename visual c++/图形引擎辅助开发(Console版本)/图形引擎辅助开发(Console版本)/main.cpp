
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "math_engine.h"
void mov(void *dest, void *res, int size);
int main(void){
	
//	FILE *file = fopen("main.cpp", "r");
//	if (file == NULL)
//	{
//		printf("open file error!");
//		return 0;
///	}
//	char buffer[128];
//	char *s = fgets(buffer, 128, file);
//	printf("buffer size:%d s size:%d", buffer, s);
	char s1[128];
	char s2[] = "abcdefg\0";
	int length = strlen(s2);
	mov(s1, s2, length);
	printf("s1:%s\n", s1);

	return 0;
}
void mov(void *dest, void *res, int size_t){
	
	unsigned char *d = (unsigned char*)dest;
	unsigned char *r = (unsigned char*)res;

	unsigned int c;
	unsigned int *count = &c;
	printf("end\n");

	__asm{
			//给d这个变量dest的地址
		//	mov eax, dword ptr [dest]
		//	mov dword ptr [d], eax
			//给r这个变量res的地址
		//	mov eax, dword ptr [res]
		//	mov dword ptr[r], eax
		//指定循环次数 
			mov eax, size_t
			add eax, 1
			mov ecx, eax
			mov dword ptr [count], 0
		loop_flag:
				mov eax, dword ptr[d]
				add eax, dword ptr[count]

				mov ebx, dword ptr[r]
				add ebx, dword ptr[count]
				//开始移动数据
				mov dl, byte ptr[ebx]
				mov byte ptr[eax], dl

				mov eax, dword ptr [count]
				add eax, 1
				mov dword ptr [count], eax
		loop loop_flag
		
	}
	/*
	__asm{
		mov eax, 0xffffffff
		mov ecx, eax
		loop_flag:
	}
	count++;
	__asm{
		loop loop_flag
		//sub cx, 1
	}
	*/
	return;
}