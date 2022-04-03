#include <iostream>
#include <vector>
using namespace std;

#define N 5
void BubbleSort(vector<int>& vec)
{
	if (vec[0])
		return;
	for (int i = 0; i < N - 1; ++i)
	{
		for (int j = i + 1; j < N; ++j)
		{
			if (vec[i] > vec[j]) 
				swap(vec[i], vec[j]);
		}
	}
	
	
}


int  main()
{
	int a;
	vector<int> vec;
	for (int i = 0; i < N; ++i)
	{
		cin >> a;
		vec.push_back(a);
	}
	BubbleSort(vec);
	for (int i = 0; i < N; ++i)
	{
		cout << vec[i] << endl;
	}
	return 0;
}
