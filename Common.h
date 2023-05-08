#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <random>
#include <memory>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

void AwaitInput()
{
	while (true)
	{
		Sleep(100);
		if (GetAsyncKeyState(VK_RETURN))
			return;
	}
}