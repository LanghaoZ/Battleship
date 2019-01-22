#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
 public:
    GameImpl(int nRows, int nCols);
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);
private:

	//This struct is used to represent a ship object

	struct ship
	{
		int shipLength;
		char shipSymbol;
		string shipName;
	};

	ship m_ships[100];

	int m_rows;
	int m_cols;
	int m_shipNumber;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
{
	m_rows = nRows;
	m_cols = nCols;
	m_shipNumber = 0;
}

int GameImpl::rows() const
{
	return m_rows;
}

int GameImpl::cols() const
{
	return m_cols;
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
	if (symbol == '.' || symbol == 'X' || symbol == 'o')
		return false;
	if (name == "")
		return false;

	for (int k = 0; k < m_shipNumber; k++)
	{
		if (symbol == m_ships[k].shipSymbol)
			return false;
	}

	const int type = m_shipNumber;
	m_shipNumber++;

	m_ships[type].shipLength = length;
	m_ships[type].shipSymbol = symbol;
	m_ships[type].shipName = name;

    return true;  // This compiles but may not be correct
}

int GameImpl::nShips() const
{
	return m_shipNumber;
}

int GameImpl::shipLength(int shipId) const  //needs check
{
	int length = m_ships[shipId].shipLength;
	
	return length;
}

char GameImpl::shipSymbol(int shipId) const  //needs check
{
	char symbol = m_ships[shipId].shipSymbol;

	return symbol;
}

string GameImpl::shipName(int shipId) const  //needs check
{
	string name = m_ships[shipId].shipName;

	return name;
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)  //needs to fix press enter to continue issue
{

	if (!p1->placeShips(b1) || !p2->placeShips(b2))
		return nullptr;

	bool shotsHitOne = false;
	bool shotsHitTwo = false;
	bool destroyedOne = false;
	bool destroyedTwo = false;
	int idOne = -1;
	int idTwo = -1;

	while (!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
	{

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                           Player 1's turn:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		cout << p1->name() << "'s turn. Board for " << p2->name() << endl;

		b2.display(p1->isHuman());

		Point p = p1->recommendAttack();

		if (!b2.attack(p, shotsHitOne, destroyedOne, idOne))
		{
			cout << p1->name() << " wasted a shot at (" << p.r << "," << p.c << ")" << endl;
			p1->recordAttackResult(p, false, false, false, -1);
		}

		else
		{
			p1->recordAttackResult(p, true, shotsHitOne, destroyedOne, idOne);

			cout << p1->name() << " attacked (" << p.r << "," << p.c << ")" << " and ";

			if (shotsHitOne)
			{
				if (!destroyedOne)
					cout << "hit something, resulting in:" << endl;

				else
					cout << "destroyed the " << shipName(idOne) << ", resulting in:" << endl;
			}
			else
				cout << "missed, resulting in:" << endl;

			b2.display(p1->isHuman());
		}

		if (shouldPause)
			waitForEnter();

		//Check if this player beats the opponent

		if (b2.allShipsDestroyed())
			break;


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                           Player 2's turn:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		cout << p2->name() << "'s turn. Board for " << p1->name() << endl;

		b1.display(p2->isHuman());

		Point b = p2->recommendAttack();

		if (b1.attack(b, shotsHitTwo, destroyedTwo, idTwo))
		{
			p2->recordAttackResult(b, true, shotsHitTwo, destroyedTwo, idTwo);

			cout << p2->name() << " attacked (" << b.r << "," << b.c << ")" << " and ";

			if (shotsHitTwo)
			{
				if (!destroyedTwo)
					cout << "hit something, resulting in:" << endl;
				else
					cout << "destroyed " << shipName(idTwo) << ", resulting in:" << endl;
			}
			else
				cout << "missed, resulting in:" << endl;

			b1.display(p2->isHuman());
		}

		else
		{
			p2->recordAttackResult(b, false, false, false, -1);

			cout << p2->name() << " wasted a shot at (" << b.r << "," << b.c << ")" << endl;
		}

		if (shouldPause)
		{
			if (p2->isHuman())
				cin.ignore(100000000000, '\n');
			waitForEnter();
		}
	}

	//Now that the game has over...

	if (b1.allShipsDestroyed())
	{
		if (p1->isHuman())
			b2.display(false);
		return p2;
	}

	if (b2.allShipsDestroyed())
	{
		if (p2->isHuman())
			b1.display(false);
		return p1;
	}


    return nullptr;  // This compiles but may not be correct
}

//******************** Game functions *******************************

// These functions for the most part simply delegate to GameImpl's functions.
// You probably don't want to change any of the code from this point down.

Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == nullptr  ||  p2 == nullptr  ||  nShips() == 0)
        return nullptr;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

