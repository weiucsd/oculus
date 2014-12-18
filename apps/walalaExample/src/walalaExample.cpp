#include <iostream>
#include "walala.h"

int main()
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile( "config.xml" ))
	{
		std::cout << "failed to load config file" << std::endl;
		return -1;
	}
	else
	{
		std::cout <<  doc.FirstChildElement( "CONFIG" )->FirstChildElement( "SETTING" )->FirstChild()->ToText()->Value() << std::endl;
	}

	int x;
	float y;

	doc.FirstChildElement( "CONFIG" )->FirstChildElement( "VALUE" )->FirstChildElement( "x" )->QueryIntText( &x );
	doc.FirstChildElement( "CONFIG" )->FirstChildElement( "VALUE" )->FirstChildElement( "y" )->QueryFloatText( &y );

	std::cout << x << std::endl;
	std::cout << y << std::endl; 

	return 0;
}
