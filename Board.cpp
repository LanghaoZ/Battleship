#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

  private:
	char gameBoard[MAXROWS][MAXCOLS];
	char shipSym[100];

	bool find(char value);

    const Game& m_game;
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
	for (int k = 0; k < MAXROWS; k++)
		for (int j = 0; j < MAXCOLS; j++)
			gameBoard[k][j] = '.';
	for (int k = 0; k < 100; k++)
		shipSym[k] = 'X';
}

bool BoardImpl::find(char value)
{
	const int row = m_game.rows();
	const int col = m_game.cols();

	for (int k = 0; k < row; k++)
	{
		for (int j = 0; j < col; j++)
		{
			if (gameBoard[k][j] == value)
				return true;
		}
	}

	return false;
}

void BoardImpl::clear()
{
	for (int k = 0; k < MAXROWS; k++)
		for (int j = 0; j < MAXCOLS; j++)
			gameBoard[k][j] = '.';
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
            if (randInt(2) == 0)
            {
                gameBoard[r][c] = 'o'; // no ship can used o as its symbol
            }
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
        for (int c = 0; c < m_game.cols(); c++)
        {
			if (gameBoard[r][c] == 'o')
				gameBoard[r][c] = '.';
        }
}

//Place the ship if the ship can be placed at
//the indicated location.

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	const int row = topOrLeft.r;
	const int col = topOrLeft.c;

	if (!m_game.isValid(topOrLeft))
		return false;

	if (shipId < 0 || shipId >= m_game.nShips())
		return false;

	char symbol = m_game.shipSymbol(shipId);
	int length = m_game.shipLength(shipId);

	if (find(symbol))
		return false;
	
	if (dir == HORIZONTAL)
	{
		int limit = col + length - 1;
		if (limit >= m_game.cols())
			return false;

		for (int k = col; k <= limit; k++)
		{
			if (gameBoard[row][k] != '.')
				return false;
		}

		for (int k = col; k <= limit; k++)
			gameBoard[row][k] = symbol;

		shipSym[shipId] = symbol;

		return true;
	}

	else if (dir == VERTICAL)
	{
		int limit = row + length - 1;
		if (limit >= m_game.rows())
			return false;

		for (int k = row; k <= limit; k++)
		{
			if (gameBoard[k][col] != '.')
				return false;
		}

		for (int k = row; k <= limit; k++)
			gameBoard[k][col] = symbol;

		shipSym[shipId] = symbol;

		return true;
	}

    return false;
}

//Remove all ship symbols from the board if there
//does exist a corresponding ship at the indicated
//location.

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	if (shipId < 0 || shipId >= m_game.nShips())
		return false;

	const char symbol = m_game.shipSymbol(shipId);
	const int length = m_game.shipLength(shipId);
	const int row = topOrLeft.r;
	const int col = topOrLeft.c;

	if (gameBoard[row][col] != symbol)
		return false;

	if (!find(symbol))
		return false;

	if (dir == HORIZONTAL)
	{
		int limit = col + length - 1;
		if (limit >= m_game.cols())
			return false;

		for (int k = col; k <= limit; k++)
		{
			if (gameBoard[row][k] != symbol)
				return false;
		}

		for (int k = 0; k < 10; k++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (gameBoard[k][j] == symbol)
					gameBoard[k][j] = '.';
			}
		}

		shipSym[shipId] = 'X';

		return true;
	}

	else if (dir == VERTICAL)
	{
		int limit = row + length - 1;
		if (limit >= m_game.rows())
			return false;

		for (int k = row; k <= limit; k++)
		{
			if (gameBoard[k][col] != symbol)
				return false;
		}

		for (int k = 0; k < 10; k++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (gameBoard[k][j] == symbol)
					gameBoard[k][j] = '.';
			}
		}

		shipSym[shipId] = 'X';

		return true;
	}

    return false;
}

//cout all elements in the two-dimensional array that
//represents the board. Depending on shotsOnly, block
//out the ship placements.

void BoardImpl::display(bool shotsOnly) const
{
	if (shotsOnly)
	{
		cout << "  ";
		for (int k = 0; k < m_game.rows(); k++)
			cout << k;
		cout << endl;
		for (int k = 0; k < m_game.rows(); k++)
		{
			cout << k << " ";

			for (int j = 0; j < m_game.cols(); j++)
			{
				switch (gameBoard[k][j])
				{
				case 'o':
				case 'X':
					cout << gameBoard[k][j];
					break;
				default:
					cout << '.';
					break;
				}
			}

			cout << endl;
		}
	}

	else
	{
		cout << "  ";
		for (int k = 0; k < m_game.rows(); k++)
			cout << k;
		cout << endl;

		for (int k = 0; k < m_game.rows(); k++)
		{
			cout << k << " ";
			for (int j = 0; j < m_game.cols(); j++)
				cout << gameBoard[k][j];
			cout << endl;
		}
	}
}

//The attacks are represented by different symbols,
//either o or X, depending on the result. If the ship
//hit does not have any more symbols on the board, it
//must have been destroyed.

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	shotHit = false;
	const int row = p.r;
	const int col = p.c;

	if (!m_game.isValid(p))
		return false;

	if (gameBoard[row][col] == 'X' || gameBoard[row][col] == 'o')
		return false;

	else if (gameBoard[row][col] == '.')
	{
		gameBoard[row][col] = 'o';
		return true;		
	}

	else
	{
		shotHit = true;
		char sym = gameBoard[row][col];
		gameBoard[row][col] = 'X';

		if (find(sym))
			shipDestroyed = false;
		else
		{
			shipDestroyed = true;
			int k = 0;

			for (; k < m_game.nShips(); k++)
			{
				if (shipSym[k] == sym)
					break;
			}

			shipId = k;
		}
		return true;
	}

    return false;
}

//If there exists no ship symbols on the board
//then all ships must have been destroyed.

bool BoardImpl::allShipsDestroyed() const
{
	for (int k = 0; k < m_game.rows(); k++)
	{
		for (int j = 0; j < m_game.cols(); j++)
		{
			switch (gameBoard[k][j])
			{
			case 'X':
			case '.':
			case 'o':
				break;
			default:
				return false;
			}
		}
	}

    return true; 
}



//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}