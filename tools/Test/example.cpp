#include <iostream>
using namespace std;

int  main()
{
	int a,b;
	int c=1;
	cin>>a;
	cin>>b;
	if (a>0)
		cin>>c;
	else
		cin>>b;
	if(a+b>0)
	{
		cout<<"a";
		return 1;
	}
	else if(a<1)
	{
		cout<<"b";
		return 2;
	}
	else
	{
		cout<<"c";
		return 3;
		}
}
