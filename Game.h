#pragma once

#include "Common.h"
#include "Board.h"
#include "ComputerBoard.h"

class Game
{
public:
	Game() = default;

	void Run()
	{
		std::cout << "Welcome to Tic-Tac-Toe\nControl the game using the arrow keys!\nPress Enter to continue\n";
		AwaitInput();
		Menu();
	}

private:

	void Menu()
	{
		const std::array<std::string, 3> menu = { "Singleplayer", "Multiplayer", "Exit" };
		const std::array<std::string, 3> adittionalInfo = { " --- Play a game against the computer\n", " --- Play a game against a friend\n", "\n" };
		int pointer = 0;
		while (true)
		{
			system("cls");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			std::cout << "Main Menu:\n\n";
			for (size_t index = 0; index < menu.size(); ++index)
			{
				if (index == 2)
					std::cout << '\n';

				if (pointer == index)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					std::cout << menu[index];
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					std::cout << adittionalInfo[index];
				}
				else
					std::cout << menu[index] << '\n';
			}

			Sleep(75);
			while (true)
			{
				if (GetAsyncKeyState(VK_UP))
				{
					--pointer;
					if (pointer == -1)
						pointer = 2;
					break;
				}
				else if (GetAsyncKeyState(VK_DOWN))
				{
					++pointer;
					if (pointer == 3)
						pointer = 0;
					break;
				}
				else if (GetAsyncKeyState(VK_ESCAPE))
				{
					EndSession();
					return;
				}
				else if (GetAsyncKeyState(VK_RETURN))
				{
					//returns whether the game has been closed, has been forcefully ended (1), or has been succesfully played (0)
					bool returnedValue = Gamemode(pointer);
					if (returnedValue)
						return;
					else
					{
						Menu();
						return;
					}
				}
			}
		}
	}

	bool Gamemode(const int gameType)
	{
		if (gameType == 0)
		{
			return Singleplayer();
		}
		else if (gameType == 1)
		{
			return Multiplayer();
		}
		else
		{
			return EndSession();
		}
	}

	bool Singleplayer()
	{
		const int COMMON_TURNS = 4;
		int result;
		std::unique_ptr<ComputerBoard> cBoard(new ComputerBoard());

		if (!cBoard->ComputerBoard::Init())
			return 0;

		if (cBoard->IsComputerFirst())
			cBoard->ComputerTurn();

		for (int turn = 0; turn < COMMON_TURNS; ++turn)
		{
			result = cBoard->PlayerTurn();
			if (result == 2) // the player can input bad positions, thus making a separate '2' value necessary for those cases
				return EndSession();
			else if (result)
				return 0;
			result = cBoard->ComputerTurn();
			if (result) // computer will either give a positive or a negative result
				return 0;

		}

		if (!(cBoard->IsComputerFirst()))
			result = cBoard->PlayerTurn();

		if (result)
			return 0;

		std::cout << "Game ended in a tie\n";
		AwaitInput();
		return 0;
	}
	// Maybe  SmartPointers??
	bool Multiplayer()
	{
		std::unique_ptr<Board> board(new Board()); // maybe SmartPointers ????
		if (!board->Init())
			return 0;

		for (int turn = 0; turn < Board::Size; ++turn)
		{
			int unused;
			int result = board->Input(turn & 1, unused); // player1 = 0, player2 = 1
			if (result == 2)
				return EndSession();
			else if (result == 1)
				return 0;
		}

		std::cout << "Game ended in a tie\n";
		AwaitInput();
		return 0;
	}

	const bool EndSession() const
	{
		system("cls");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
		std::cout << "Thanks for playing <3\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

		AwaitInput();
		return 1;
	}
};