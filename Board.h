#pragma once

#include "Common.h"

class Board
{
public:
	static constexpr int Size = 9;
	static constexpr int ComputerPlayer = 101;

	Board() {
		std::fill(m_BoardState.begin(), m_BoardState.end(), ' ');
	}

	bool Init() const
	{
		system("cls");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		DisplayClearBoard();
		return AwaitAccept();
	}

	int Input(const int player, int& position) // player 1 = 0 ('X'), Player 2 = 1 ('O'); sp Player = -1 ('O'), sp Player = -2 ('X');
	{
		Coord pointer;
		int failCounter = 0;
		char symbol = 0;
		if (player == 0 || player == -2)
			symbol = 'X';
		else
			symbol = 'O';

		while (failCounter != 3)
		{
			DisplayBoard(pointer, symbol, player);

			if (failCounter)
				std::cout << "Please select an unused position\n";

			Sleep(75);
			while (true)
			{
				if (GetAsyncKeyState(VK_DOWN))
				{
					if (pointer.y != 2)
						++pointer.y;
					break;
				}
				else if (GetAsyncKeyState(VK_UP))
				{
					if (pointer.y)
						--pointer.y;
					break;
				}
				else if (GetAsyncKeyState(VK_LEFT))
				{
					if (pointer.x)
						--pointer.x;
					break;
				}
				else if (GetAsyncKeyState(VK_RIGHT))
				{
					if (pointer.x != 2)
						++pointer.x;
					break;
				}
				else if (GetAsyncKeyState(VK_ESCAPE))
				{
					failCounter = 3;
					pointer.x = -1;
					break;
				}
				else if (GetAsyncKeyState(VK_RETURN))
				{
					if (ProcessInput(pointer.Value(), symbol))
					{
						if (VerifyBoard(symbol, player))
						{
							AwaitInput();
							return 1;
						}

						position = pointer.Value();
						return 0;
					}
					++failCounter;
					break;
				}
			}
		}
		std::cout << "Game session ended\nPlayer ";
		if (player > -1)
		{
			std::cout << player + 1 << ' ';
		}
		std::cout << "forcibly closed the session\n";
		AwaitInput();
		if (pointer.x == -1)
			return 1;
		return 2;
	}

protected:
	struct Coord
	{
		Coord() = default;

		int x = 1;
		int y = 1;

		Coord(const int expandedPosition) : Coord() {
			x = expandedPosition % 3;
			y = expandedPosition / 3;
		}

		Coord(const int x0, const int y0) : Coord() {
			x = x0;
			y = y0;
		}

		int WhereOnThisRow(const int value) const
		{
			for (int i = 0; i < 3; ++i)
			{
				if (Value() == value + i)
					return i;
			}
			return -1;
		}

		int Value() const
		{
			return x + y * 3;
		}

		bool const operator == (const int other) const
		{
			return Value() == other;
		}

	};

	bool ComputerInput(const int chosenPosition, const char computerSymbol)
	{
		ProcessInput(chosenPosition, computerSymbol);
		if (VerifyBoard(computerSymbol, ComputerPlayer))
		{
			AwaitInput();
			return 1;
		}
		return 0;
	}

	bool AwaitAccept() const
	{
		Sleep(75);
		while (true)
		{
			if (GetAsyncKeyState(VK_ESCAPE))
				return 0;
			else if (GetAsyncKeyState(VK_RETURN))
				return 1;
		}
	}

	void DisplayBoard(const Coord& pointer, const char symbol, const int player) const
	{
		ClearConsole();
		SelectPrompt(player);
		TopRow();
		NumberedRow(0, pointer, symbol);
		MidRow();
		NumberedRow(3, pointer, symbol);
		MidRow();
		NumberedRow(6, pointer, symbol);
		BotRow();
	}

	void ClearConsole() const
	{
		system("cls");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	}

	std::array<char, Size> m_BoardState;

private:

	static constexpr bool Player1 = 0;
	static constexpr bool Player2 = 1;

	static constexpr std::array<char, 2> m_Symbols = { 'X', 'O' };

	void SelectPrompt(const int player) const
	{
		if (player == ComputerPlayer) {
			std::cout << "Computer is making it's move\n";
			return;
		}
		std::cout << "Player";
		if (player > -1)
		{
			std::cout << ' ' << player + 1;
		}
		std::cout << ", please select a board position\n";
	}

	bool ProcessInput(const int value, const char playerSymbol)
	{
		if (m_BoardState[value] == ' ')
		{
			m_BoardState[value] = playerSymbol;
			return 1;
		}
		return 0;
	}

	bool VerifyBoard(const char playerSymbol, const int player) const
	{
		for (int begin = 0; begin != 9; begin += 3)
			if (CheckRow(begin, playerSymbol))
			{
				WinningMessage(player);
				std::cout << "by completing a row!!!\n";
				return 1;
			}
		for (int begin = 0; begin != 3; ++begin)
			if (CheckColumn(begin, playerSymbol))
			{
				WinningMessage(player);
				std::cout << "by completing a column!!!\n";
				return 1;
			}
		if (CheckDiagonals(playerSymbol))
		{
			WinningMessage(player);
			std::cout << "by completing a diagonal!!!\n";
			return 1;
		}
		return 0;
	}

	bool CheckRow(const int begin, const char playerSymbol) const
	{
		return playerSymbol == m_BoardState[begin] && m_BoardState[begin] == m_BoardState[(size_t)begin + 1] && m_BoardState[(size_t)begin + 1] == m_BoardState[(size_t)begin + 2];
	}

	bool CheckColumn(const int begin, const char playerSymbol) const
	{
		return playerSymbol == m_BoardState[begin] && m_BoardState[begin] == m_BoardState[(size_t)begin + 3] && m_BoardState[(size_t)begin + 3] == m_BoardState[(size_t)begin + 6];
	}

	bool CheckDiagonals(const char playerSymbol) const
	{
		if (PrimaryDiagonal(playerSymbol))
			return 1;
		if (SecondaryDiagonal(playerSymbol))
			return 1;
		return 0;
	}

	bool PrimaryDiagonal(const char playerSymbol) const
	{
		return playerSymbol == m_BoardState[0] && m_BoardState[0] == m_BoardState[4] && m_BoardState[4] == m_BoardState[8];
	}

	bool SecondaryDiagonal(const char playerSymbol) const
	{
		return playerSymbol == m_BoardState[2] && m_BoardState[2] == m_BoardState[4] && m_BoardState[4] == m_BoardState[6];
	}

	bool WinningMessage(const int player) const
	{
		if (player == ComputerPlayer)
		{
			std::cout << "Computer has won ";
			return 1;
		}
		std::cout << "Player ";
		if (player > -1)
			std::cout << player + 1 << ' ';
		std::cout << "has won ";
		return 1;
	}

	void NumberedRow(const int begin, const Coord& pointer, const char symbol) const
	{
		int selectedPosition = pointer.WhereOnThisRow(begin);
		if (selectedPosition == -1)
		{
			SimpleNumberedRow(begin);
			return;
		}
		std::cout << "| ";
		if (selectedPosition == 0)
		{
			SelectedPosition(begin, symbol);
		}
		else
		{
			std::cout << m_BoardState[begin];
		}
		std::cout << " | ";
		if (selectedPosition == 1)
		{
			SelectedPosition(begin + 1, symbol);
		}
		else
		{
			std::cout << m_BoardState[(size_t)begin + 1];
		}
		std::cout << " | ";
		if (selectedPosition == 2)
		{
			SelectedPosition(begin + 2, symbol);
		}
		else
		{
			std::cout << m_BoardState[(size_t)begin + 2];
		}
		std::cout << " |\n";
	}

	void SelectedPosition(const int index, const char symbol) const
	{
		if (m_BoardState[index] == ' ')
		{

			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
			std::cout << symbol;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
		else
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			std::cout << m_BoardState[index];
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		}
	}

	void SimpleNumberedRow(const int begin) const
	{
		std::cout << "| ";
		std::cout << m_BoardState[begin];
		std::cout << " | ";
		std::cout << m_BoardState[(size_t)begin + 1];
		std::cout << " | ";
		std::cout << m_BoardState[(size_t)begin + 2];
		std::cout << " |\n";
	}

	void TopRow() const
	{
		std::cout << "ÚÄÄÄÂÄÄÄÂÄÄÄ¿\n";
	}

	void MidRow() const
	{
		std::cout << "ÃÄÄÄÅÄÄÄÅÄÄÄ´\n";
	}

	void BotRow() const
	{
		std::cout << "ÀÄÄÄÁÄÄÄÁÄÄÄÙ\n";
	}

	void DisplayClearBoard() const
	{
		std::cout << "When prompted, using the arrow keys, select the chosen board position\nPress enter to continue\nPress escape to return\n";

		std::cout << "ÚÄÄÄÂÄÄÄÂÄÄÄ¿\n";
		std::cout << "|   |   |   |\n";
		std::cout << "ÃÄÄÄÅÄÄÄÅÄÄÄ´\n";
		std::cout << "|   |   |   |\n";
		std::cout << "ÃÄÄÄÅÄÄÄÅÄÄÄ´\n";
		std::cout << "|   |   |   |\n";
		std::cout << "ÀÄÄÄÁÄÄÄÁÄÄÄÙ\n";
	}

};
