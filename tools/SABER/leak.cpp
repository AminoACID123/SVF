#include<stdio.h>
#include<stdlib.h>

void BubbleSort(int a[], int len)
{
	int i, j, temp;
	for (j = 0; j < len - 1; j++)
	{
		for (i = 0; i < len - 1 - j; i++)
		if (a[i] > a[i + 1])
		{
			temp = a[i];
			a[i] = a[i + 1];
			a[i + 1] = temp;
		}
	}
	//return 1;
}


void _malloc2(char* p)
{
	p = (char*)malloc(10);
}

void _malloc1(char* p)
{
	_malloc2(p);
}



 
int main()
{
	int arr[] = { 5, 8, 6, 3, 9, 2, 1, 7 };
	int len = sizeof(arr) / sizeof(arr[0]);
	int i = 0;
	char* t;
 	_malloc1(t);
	BubbleSort(arr, len);
	free(t);
	
	return 0;
}
