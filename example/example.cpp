#include <iostream>
using namespace std;


int add(int& a, int& b)
{
	int c;
	a++;
	b++;
	c = a + b;
	return c;
}


int main()
{
	int x,y,z,a;
	x = 1;
	if(y>3)
		add(x,z);
	else
		add(x,y);
	y = 2;
	a = x;
	z = add(x,y);
	return z;
}






