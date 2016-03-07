#ifndef GAME_H
#define GAME_H

#include "MiniGameInterface.hpp"

struct Cell
{
	int i;
	int j;

	Cell()
	{
	}

	Cell(int i, int j)
	{
		this->i = i;
		this->j = j;
	}

	bool operator==(const Cell &cell) const
	{
		return i == cell.i && j == cell.j;
	}

	bool operator!=(const Cell &cell) const
	{
		return !(*this == cell);
	}
};

class Game : public MiniGame
{
private:
	float textureRectWidth;
	float textureRectHeight;
	float screenRectWidth;
	float screenRectHeight;
	float screenWidth;
	float screenHeight;
	Cell selected;
	int field[cRows][cColumns];

	Rect restartButtonScreenCoords;

	Cell swappingCell[2];
	bool swapping;
	float swappingDelta;
	int swappingEnding;

	RECT ScreenRect;
	Rect pointerRect;
	

public:
	static GDIWorker* mmgdi; //This object is static,because it will use in global DrawRect func and some manipulations in main
	Game();
	~Game();
	void Initialize();
	void Click(float x, float y);
	bool IsCompleted() const;
	void Render() const;

	int CountMatches() const;
	void UpdateEffects();

	int GetPieceNumber(int i, int j) const;
	Cell GetPieceIndex(int pieceNumber) const;
	
private:
	void UpdateSwapping();
	void SwapCells(Cell& cell0, Cell& cell1);
};

#endif