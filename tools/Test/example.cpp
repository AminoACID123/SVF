#include <iostream>
#include <vector>
using namespace std;

int  main()
{
	vector<int> vec;
	int n = vec.size();
	for (int i = 0; i < n; ++i)
	{
		cin >> vec[i];
	}
	
	for (int i = 0; i < n - 1; ++i)
	{
		for (int j = i + 1; j < n; ++j)
		{
			if (vec[i] > vec[j]) 
				swap(vec[i], vec[j]);
		}
	}

}
