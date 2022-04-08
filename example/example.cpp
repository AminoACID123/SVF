#include <iostream>
using namespace std;

int A = 1;

int foo(int a, int b)
{
	int c;
	c = a + b;
	return c;
}


int main()
{
	int x,y,z;
	cin>>x;
	cin>>y;
	z = foo(x,y);
	return z;
}
