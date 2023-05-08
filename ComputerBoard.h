#pragma once

#include "Common.h"
#include "Board.h"

class ComputerBoard : Board
{
public:
	ComputerBoard() = default;

	bool Init()
	{
	difficulty:
		if (!SelectDifficulty())
			return 0;
	symbol:
		if (!SelectSymbol())
			goto difficulty;
		if (!Board::Init())
			goto symbol;
		else
			return true;
	}

	bool IsComputerFirst() const
	{
		return m_RobotTurn;
	}

	int PlayerTurn()
	{
		// result of player input operation
		int result = Input((int)m_RobotTurn - 2, m_LastPlayerMove); // parameter value of -1 means player has 'O', -2 means player has 'X'
		if (result)
			return result;

		RemoveChosenElement(IndexOfElement(m_LastPlayerMove));
		++m_CurrentMove;
		return 0;
	}

	bool ComputerTurn()
	{
		int index;
		const char computerSymbol = m_RobotTurn ? 'X' : 'O';

		if (m_Difficulty == 0)
			index = RNG(m_PositionsLeft.size());
		else if (m_Difficulty == 1)
			index = ComputerAdvanced(computerSymbol);
		else
			index = ComputerSmart(computerSymbol);  // NOT IMPLEMENTED YET

		m_LastComputerMove = m_PositionsLeft[index];
		++m_CurrentMove;
		DisplayMovement(m_PositionsLeft[index], computerSymbol);

		if (ComputerInput(m_PositionsLeft[index], computerSymbol))
			return 1;

		RemoveChosenElement(index);
		return 0;
	}

private:

	bool SelectDifficulty()
	{
		int pointer = 0;
		static const std::array<std::string, 4> difficulty = { "Beginner", "Advanced", "Expert", "Return" };
		static const std::array<std::string, 4> additionalInfo = { " --- Randomized movement\n", " --- Semi-Random behaviour\n", "   --- Calculated behaviour\n", "\n" };

		while (true)
		{
			ClearConsole();
			std::cout << "Select a difficulty level:\n\n";

			for (int index = 0; index < difficulty.size(); ++index)
			{
				if (index == 3)
					std::cout << '\n';

				if (pointer == index)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					std::cout << difficulty[index];
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					std::cout << additionalInfo[index];
				}
				else
					std::cout << difficulty[index] << '\n';
			}

			Sleep(75);
			while (true)
			{
				if (GetAsyncKeyState(VK_DOWN))
				{
					++pointer;
					if (pointer == 4)
						pointer = 0;
					break;
				}
				else if (GetAsyncKeyState(VK_UP))
				{
					--pointer;
					if (pointer == -1)
						pointer = 2;
					break;
				}
				else if (GetAsyncKeyState(VK_ESCAPE))
				{
					return 0;
				}
				else if (GetAsyncKeyState(VK_RETURN))
				{
					if (pointer == 3)
					{
						return 0;
					}

					m_Difficulty = pointer;
					return 1;
				}
			}
		}

	}

	bool SelectSymbol()
	{
		int pointer_x = 0;
		int pointer_y = 0;
		const std::array<char, 2> symbols = { 'X', 'O' };
		const std::string Return = "Return";

		while (true)
		{
			system("cls");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
			std::cout << "Select your symbol:\n\n";

			for (int index = 0; index < symbols.size(); ++index)
			{
				if (index)
				{
					std::cout << "  ";
				}
				if (!pointer_y && pointer_x == index)
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
					std::cout << symbols[index];
				}
				else
				{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
					std::cout << symbols[index];
				}
			}
			std::cout << "\n\n";
			if (pointer_y)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
				std::cout << Return << '\n';
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				std::cout << Return << '\n';
			}

			std::cout << '\n';

			Sleep(75);
			while (true)
			{
				if (GetAsyncKeyState(VK_LEFT))
				{
					if (pointer_x)
						--pointer_x;
					break;
				}
				else if (GetAsyncKeyState(VK_RIGHT))
				{
					if (!pointer_x)
						++pointer_x;
					break;
				}
				else if (GetAsyncKeyState(VK_DOWN))
				{
					if (!pointer_y)
						++pointer_y;
					break;
				}
				else if (GetAsyncKeyState(VK_UP))
				{
					if (pointer_y)
						--pointer_y;
					break;
				}
				else if (GetAsyncKeyState(VK_ESCAPE))
				{
					return 0;
				}
				else if (GetAsyncKeyState(VK_RETURN))
				{
					if (pointer_y)
					{
						return 0;
					}
					m_RobotTurn = pointer_x;
					return 1;
				}
			}
		}
	}

	int RNG(const size_t& mod) const
	{
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<int> distribution(0, 9999);

		return distribution(generator) % mod;
	}

	int ComputerAdvanced(const char computerSymbol) const
	{
		int result = CheckOptions(computerSymbol);
		if (result == -1)
			result = RNG(m_PositionsLeft.size());
		return result;
	}

	int CheckOptions(const char symbol) const
	{
		int blockIndex = -1;

		for (int begin = 0; begin != 9; begin += 3)
		{
			if (const auto& [index, winning] = CheckRows(begin, symbol); index != -1)
			{
				if (winning)
					return index;

				blockIndex = index;
			}
		}
		for (int begin = 0; begin != 3; ++begin)
		{
			if (const auto& [index, winning] = CheckColumns(begin, symbol); index != -1)
			{
				if (winning)
					return index;

				blockIndex = index;
			}
		}

		if (const auto& [index, winning] = CheckDiagonals(symbol); index != -1)
		{
			if (winning)
				return index;

			blockIndex = index;
		}

		return blockIndex;
	}

	std::pair<int, bool> CheckRows(const int begin, const char symbol) const
	{
		int frSymbol = 0;		// counts number of computer symbols
		int frEmpty = 0;		// counts empty spaces
		int emptySlot = 0;		// if there is only one space left in a row, this will contain the position of the last element
		for (int i = 0; i != 3; ++i)
		{
			if (m_BoardState[(size_t)begin + i] == symbol)
				++frSymbol;
			else if (m_BoardState[(size_t)begin + i] == ' ')
			{
				emptySlot = begin + i;
				++frEmpty;
			}
		}

		if (IsLineFinishable(frSymbol, frEmpty))
		{
			int index = IndexOfElement(emptySlot);
			if (IsLineWinning(frSymbol))
				return { index, true };

			return { index, false };
		}

		return { -1, false };
	}

	std::pair<int, bool> CheckColumns(const int begin, const char symbol) const
	{
		int frSymbol = 0;
		int frEmpty = 0;
		int emptySlot = 0;
		for (int i = 0; i != 9; i += 3)
		{
			if (m_BoardState[(size_t)begin + i] == symbol)
				++frSymbol;
			else if (m_BoardState[(size_t)begin + i] == ' ')
			{
				emptySlot = begin + i;
				++frEmpty;
			}
		}
		if (IsLineFinishable(frSymbol, frEmpty))
		{
			int index = IndexOfElement(emptySlot);
			if (IsLineWinning(frSymbol))
				return { index, true };

			return { index, false };
		}

		return { -1, false };
	}

	std::pair<int, bool> CheckDiagonals(const char symbol) const
	{
		int blockIndex = -1;

		if (const auto& [index, winning] = CheckPrimaryDiagonal(symbol); index != -1)
		{
			if (winning)
				return { index, winning };

			blockIndex = index;
		}

		if (const auto& [index, winning] = CheckSecondaryDiagonal(symbol); index != -1)
		{
			if (winning)
				return { index, winning };

			blockIndex = index;
		}

		return { blockIndex, false };
	}

	std::pair<int, bool> CheckPrimaryDiagonal(const char symbol) const
	{
		int frSymbol = 0;
		int frEmpty = 0;
		int emptySlot = 0;
		for (int i = 0; i < 9; i += 4)
		{
			if (m_BoardState[i] == symbol)
				++frSymbol;
			else if (m_BoardState[i] == ' ')
			{
				emptySlot = i;
				++frEmpty;
			}
		}
		if (IsLineFinishable(frSymbol, frEmpty))
		{
			int index = IndexOfElement(emptySlot);
			if (IsLineWinning(frSymbol))
				return { index, true };

			return { index, false };
		}

		return { -1, false };
	}

	std::pair<int, bool> CheckSecondaryDiagonal(const char symbol) const
	{
		int frSymbol = 0;
		int frEmpty = 0;
		int emptySlot = 0;
		for (int i = 2; i < 7; i += 2)
		{
			if (m_BoardState[i] == symbol)
				++frSymbol;
			else if (m_BoardState[i] == ' ')
			{
				emptySlot = i;
				++frEmpty;
			}
		}
		if (IsLineFinishable(frSymbol, frEmpty))
		{
			int index = IndexOfElement(emptySlot);
			if (IsLineWinning(frSymbol))
				return { index, true };

			return { index, false };
		}

		return { -1, false };
	}

	bool IsLineFinishable(const int frSymbol, const int frEmpty) const
	{
		return frSymbol != 1 && frEmpty == 1;
	}

	bool IsLineWinning(const int frSymbol) const
	{
		return frSymbol == 2;
	}

	int ComputerSmart(const char symbol) const
	{
		if (!m_CurrentMove)
			return RandomCorner();
		else if (m_CurrentMove == 1)
			return CurrentMove1();
		else if (m_CurrentMove == 2)
			return CurrentMove2();
		else if (m_CurrentMove == 3)
			return CurrentMove3(symbol);

		return ComputerAdvanced(symbol);
	}

	int CurrentMove1() const
	{
		if (Middle())
			return IndexOfElement(RandomCorner());
		else
			return IndexOfElement(4);
	}

	int CurrentMove2() const
	{
		if (OppositeCorner() || Middle())
		{
			std::array<int, 2> possibleMoves = TouchingPosition(m_LastComputerMove);
			return IndexOfElement(possibleMoves[RNG(2)]);
		}
		else if (OppositeSide())
			return IndexOfElement(RandomNonOppositeCorner(m_LastComputerMove));
		else if (const int i = SameSide(); i) // i represents the other side
			return IndexOfElement(i);
		else // SameSidedCorners
			return IndexOfElement(TouchingSameSide());
	}

	int CurrentMove3(const char symbol) const
	{
		if (IsCorner(m_LastPlayerMove) && m_BoardState[m_LastPlayerMove] == m_BoardState[FindOppositeCorner(m_LastPlayerMove)])
		{
			return IndexOfElement(RandomNonCorner());
		}

		return ComputerAdvanced(symbol);
	}

	bool IsCorner(const int lastPlayerMove) const
	{
		for (const int corner : CornerCoords)
		{
			if (corner == lastPlayerMove)
				return 1;
		}
		return 0;
	}

	int RandomNonCorner() const
	{
		return SideCoords[RNG(SideCoords.size())];
	}

	int RandomCorner() const
	{
		return CornerCoords[RNG(CornerCoords.size())];
	}

	bool OppositeCorner() const
	{
		return m_LastComputerMove + m_LastPlayerMove == 8;
	}

	bool Middle() const
	{
		return m_LastPlayerMove == 4;
	}

	bool OppositeSide() const
	{
		int oppositeCorner = FindOppositeCorner(m_LastComputerMove);
		std::array<int, 2> touchingOppositeSides = TouchingPosition(oppositeCorner);

		for (int index = 0; index != touchingOppositeSides.size(); ++index)
			if (m_LastPlayerMove == touchingOppositeSides[index])
				return 1;
		return 0;
	}

	int FindOppositeCorner(const int otherCorner) const
	{
		return 8 - otherCorner;
	}

	int SameSide() const
	{
		std::array<int, 2> touchingWithComputer = TouchingPosition(m_LastComputerMove);
		for (int index = 0; index != touchingWithComputer.size(); ++index)
			if (m_LastPlayerMove == touchingWithComputer[index])
				return touchingWithComputer[!index];
		return 0;
	}

	std::array<int, 2> TouchingPosition(const int position) const
	{
		std::array<int, 2> possibleMoves = { 0, 0 };
		switch (position)
		{
		case 0:
			possibleMoves = { 1, 3 };
			break;
		case 2:
			possibleMoves = { 1, 5 };
			break;
		case 6:
			possibleMoves = { 7, 3 };
			break;
		case 8:
			possibleMoves = { 7, 5 };

		}
		return possibleMoves;
	}

	int RandomNonOppositeCorner(const int reference) const
	{
		switch (reference)
		{
		case 0:
		case 8:
		{
			std::array<int, 2> options = { 2, 6 };
			return options[RNG(options.size())];
		}
		case 2:
		case 6:
		{
			std::array<int, 2> options = { 0, 8 };
			return options[RNG(options.size())];
		}
		}

		throw std::invalid_argument("Bad reference");
	}

	int TouchingSameSide() const
	{
		Coord computerPosition(m_LastComputerMove);
		Coord playerPosition(m_LastPlayerMove);
		std::array<int, 2> touchingPositions = TouchingPosition(m_LastComputerMove);
		if (playerPosition.x == computerPosition.x)
			return touchingPositions[0];
		else
			return touchingPositions[1];
	}

	int IndexOfElement(const int removedElement) const
	{
		for (int index = 0; index != m_PositionsLeft.size(); ++index)
		{
			if (m_PositionsLeft[index] == removedElement)
				return index;
		}
		throw std::invalid_argument("Not sure how this happened bro");
	}

	void DisplayMovement(const int position, const char computerSymbol) const
	{
		Coord target(position);
		Coord current;

		DisplayBoard(current, computerSymbol, ComputerPlayer);
		Sleep(500);

		if (current.x < target.x)
		{
			++current.x;
			DisplayBoard(current, computerSymbol, ComputerPlayer);

			Sleep(500);
		}
		else if (current.x > target.x)
		{
			--current.x;
			DisplayBoard(current, computerSymbol, ComputerPlayer);

			Sleep(500);
		}

		if (current.y < target.y)
		{
			++current.y;
			DisplayBoard(current, computerSymbol, ComputerPlayer);

			Sleep(500);
		}
		else if (current.y > target.y)
		{
			--current.y;
			DisplayBoard(current, computerSymbol, ComputerPlayer);

			Sleep(500);
		}

		Sleep(300);
	}

	void RemoveChosenElement(const int index)
	{
		std::vector<int>::iterator itr = m_PositionsLeft.begin();
		std::advance(itr, index);
		m_PositionsLeft.erase(itr);
	}

	std::vector<int> m_PositionsLeft{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
	static constexpr std::array<int, 4> CornerCoords = { 0, 2, 6, 8 }; // corner coordinates
	static constexpr std::array<int, 4> SideCoords = { 1, 3, 5, 7 };
	int m_Difficulty = 0;
	bool m_RobotTurn = 0; // 0 means robot has 'O', 1 means robot has 'X'
	int m_CurrentMove = 0;
	int m_LastPlayerMove = -1;
	int m_LastComputerMove = -1;
};