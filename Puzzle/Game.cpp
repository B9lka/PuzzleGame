#include <utility>
#include <windows.h>
#include <ctime>
#include "GDIWorker.hpp"
#include "ResourceManager.hpp"
#include "Game.hpp"

Game::Game()
{
	ResourceManager::GetInstance().AddTexture(L"textures\\pointer.png");
	//ResourceManager::GetInstance().AddTexture(L"textures\\Delighted by Light II.jpg");
	//ResourceManager::GetInstance().AddTexture(L"textures\\Basket of Love.jpg");
	ResourceManager::GetInstance().AddTexture(L"textures\\novgorod.jpg");
	ResourceManager::GetInstance().AddTexture(L"textures\\packman.jpg");
	ResourceManager::GetInstance().AddTexture(L"textures\\youwon.png");
	ResourceManager::GetInstance().AddTexture(L"textures\\restart.png");
	ResourceManager::GetInstance().LoadTextures();
}

Game::~Game()
{
	ResourceManager::GetInstance().DestroyTextures();
	if (mmgdi)
		delete(mmgdi);
}

GDIWorker* Game::mmgdi; //declaration for static object

void Game::Initialize()
{
	Texture* texture = ResourceManager::GetInstance().GetTexture(cTextureId);
	GetClientRect(GetActiveWindow(), &ScreenRect);
	
	textureRectWidth = texture->GetWidth() / (float)cColumns;
	textureRectHeight = texture->GetHeight() / (float)cRows;
	
	screenRectWidth = (float)ScreenRect.right / (float)cColumns;
	screenRectHeight = (float)ScreenRect.bottom / (float)cRows;
	
	screenWidth = (float)ScreenRect.right;
	screenHeight = (float)ScreenRect.bottom;
	
	float restartWidth = (float)ScreenRect.right / 7;
	float restartHeight = (float)ScreenRect.bottom / 7;
	
	restartButtonScreenCoords.left = screenWidth / 2 - restartWidth;
	restartButtonScreenCoords.top = screenHeight / 2 - restartHeight;
	restartButtonScreenCoords.right = (screenWidth + restartWidth) / 4;
	restartButtonScreenCoords.bottom = (screenHeight + restartHeight) / 4;


	selected.i = -1;
	selected.j = -1;
	
	// game field randomizing
	srand(time(NULL));
	do
	{
		std::memset(field, -1, sizeof(int) * cRows * cColumns);
		int k = 0;
		while (k < cRows * cColumns)
		{
			int i = rand() % cRows;
			int j = rand() % cColumns;
			while (field[i][j] != -1)
			{
				int nextPiece = GetPieceNumber(i, j) + 1;
				if (nextPiece >= cColumns * cRows)
				{
					i = 0;
					j = 0;
					continue;
				}
				Cell pieceIndex = GetPieceIndex(nextPiece);
				i = pieceIndex.i;
				j = pieceIndex.j;
			}

			field[i][j] = k;
			++k;
		}
	} while (CountMatches() >= (cRows * cColumns) / 2); // we don't need (almost) completed game at start

	// configuring effects/features
	swapping = false;
	
	mmgdi = new GDIWorker(GetActiveWindow());	// definition of static object

	Texture* pointerTexture = ResourceManager::GetInstance().GetTexture(0);
	pointerRect.left = 0.0f;
	pointerRect.top = 0.0f;
	pointerRect.right = pointerTexture->GetWidth();
	pointerRect.bottom = pointerTexture->GetHeight();
}

void Game::Click(float x, float y)
{
	if (IsCompleted())
	{
		if (x >= restartButtonScreenCoords.left && x <= restartButtonScreenCoords.right &&
			y >= restartButtonScreenCoords.top && y <= restartButtonScreenCoords.bottom)
		{
			Initialize();
		}
		return;
	}

	Cell cell;
	cell.i = (int)((y * (float)cRows) / screenHeight);
	cell.j = (int)((x * (float)cColumns) / screenWidth);

	if (selected == cell)
		return;

	if (selected.i == -1)
	{
		selected.i = cell.i;
		selected.j = cell.j;
	}
	else {
		SwapCells(cell, selected);
	}
}

bool Game::IsCompleted() const
{
	int k = 0;
	for (int i = 0; i < cRows; i++)
	{
		for (int j = 0; j < cColumns; j++)
		{
			if (field[i][j] != k)
				return false;
			k++;
		}
	}
	return true;
}

void Game::Render() const
{
	mmgdi->Clear(Gdiplus::Color::Black);
	
	// playing field
	Rect screenCoords;
	Rect textureCoord;
	for (int i = 0; i < cRows; i++)
	{
		for (int j = 0; j < cColumns; j++)
		{
			screenCoords.left = j * screenRectWidth;
			screenCoords.top = i * screenRectHeight;
			screenCoords.right =  screenRectWidth;
			screenCoords.bottom =  screenRectHeight;

			Rect pointerScreenCoords = screenCoords;

			// swap effect
			Cell currentCell(i, j);
			if (swapping && (swappingCell[0] == currentCell || swappingCell[1] == currentCell))
			{
				screenCoords.left += (swappingDelta * screenRectWidth) / 100.0f;
				screenCoords.top += (swappingDelta * screenRectHeight) / 100.0f;
				screenCoords.right -= (swappingDelta *2* screenRectWidth) / 100.0f;
				screenCoords.bottom -= (swappingDelta *2* screenRectHeight) / 100.0f;
			}

			// get cell index in texture
			Cell index = GetPieceIndex(field[i][j]);

			textureCoord.left = index.j * textureRectWidth;
			textureCoord.top = index.i * textureRectHeight;
			textureCoord.right =  textureRectWidth;
			textureCoord.bottom =  textureRectHeight;

			DrawRect(screenCoords, cTextureId, textureCoord);

			// pointer
			if (!swapping && selected == currentCell)
				DrawRect(pointerScreenCoords, 0, pointerRect);
		}
	}

	// Draw restart button
	if (IsCompleted())
	{
		mmgdi->DrawTexture(restartButtonScreenCoords, 4);
	}
	mmgdi->Redraw();
}

int Game::CountMatches() const
{
	int result = 0;
	int k = 0;
	for (int i = 0; i < cRows; i++)
		for (int j = 0; j < cColumns; j++)
		{
			if (field[i][j] == k)
				result++;
			k++;
		}
	return result;
}

void Game::SwapCells(Cell& cell0, Cell& cell1)
{
	swappingCell[0] = cell0;
	swappingCell[1] = cell1;
	swapping = true;
	swappingEnding = false;
	swappingDelta = 0.0f;
}

void Game::UpdateSwapping()
{
	for (int i = 0; i < 10;i++)
	{
		if (!swapping)
			return;

		if (!swappingEnding)
			swappingDelta += 5.0f;
		else
			swappingDelta -= 5.0f;

		if (!swappingEnding && swappingDelta >= 25.0f)
		{
			int temp = field[swappingCell[0].i][swappingCell[0].j];
			field[swappingCell[0].i][swappingCell[0].j] = field[selected.i][selected.j];
			field[selected.i][selected.j] = temp;
			selected.i = -1;
			selected.j = -1;
			swappingDelta = 25.0f;
			swappingEnding = true;
		}

		if (swappingEnding && swappingDelta <= 0.0f)
		{
			swapping = false;
		}

		Game::Render();
	}
}

void Game::UpdateEffects()
{
	UpdateSwapping();
}

int Game::GetPieceNumber(int i, int j) const
{
	return (i + 1) * cColumns - (cColumns - j);
}

Cell Game::GetPieceIndex(int pieceNumber) const
{
	Cell result;
	result.i = pieceNumber / cColumns;
	result.j = pieceNumber - cColumns * result.i;
	return result;
}

void DrawRect(const Rect& screenCoords, int textureId, const Rect& textureCoord)
{
	Game::mmgdi->DrawTexture(screenCoords, textureId, textureCoord);
}
