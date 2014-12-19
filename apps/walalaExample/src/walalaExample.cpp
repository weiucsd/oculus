#include <iostream>
#include "walala.h"

int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		std::cout << "please append the input path to your image behind ./walalaExample" << std::endl;
		return -1;
	}

	VPL::walala walalaobj;

	walalaobj.TestShowImage(argv[1]); 
	walalaobj.TestPrint(); 

	return 0;
}
