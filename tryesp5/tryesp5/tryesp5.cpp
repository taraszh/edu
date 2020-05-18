
#include <iostream>
#include <Windows.h>
#include <sstream>
#include <iostream>

#include "Hack.h"

int main()
{
	Hack hack;
	hack.setBrightness();
	
	std::cout << "it is alive" << std::endl;

	while (true)
	{
		if (GetKeyState(VK_NUMPAD0) & 1)
			break;

		hack.glowEsp();
		
		Sleep(16);
	}

}
