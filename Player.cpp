#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <chrono>

using namespace std;

//*********************************************************************
//  Timer Code
//*********************************************************************

//========================================================================
// Timer t;                 // create a timer and start it
// t.start();               // start the timer
// double d = t.elapsed();  // milliseconds since timer was last started
//========================================================================

class Timer
{
public:
	Timer()
	{
		start();
	}
	void start()
	{
		m_time = std::chrono::high_resolution_clock::now();
	}
	double elapsed() const
	{
		std::chrono::duration<double, std::milli> diff =
			std::chrono::high_resolution_clock::now() - m_time;
		return diff.count();
	}
private:
	std::chrono::high_resolution_clock::time_point m_time;
};

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
public:
	AwfulPlayer(string nm, const Game& g);
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);
private:
	Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
	: Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
	// Clustering ships is bad strategy
	for (int k = 0; k < game().nShips(); k++)
		if (!b.placeShip(Point(k, 0), k, HORIZONTAL))
			return false;
	return true;
}

Point AwfulPlayer::recommendAttack()
{
	if (m_lastCellAttacked.c > 0)
		m_lastCellAttacked.c--;
	else
	{
		m_lastCellAttacked.c = game().cols() - 1;
		if (m_lastCellAttacked.r > 0)
			m_lastCellAttacked.r--;
		else
			m_lastCellAttacked.r = game().rows() - 1;
	}
	return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
	bool /* shotHit */, bool /* shipDestroyed */,
	int /* shipId */)
{
	// AwfulPlayer completely ignores the result of any attack
}

void AwfulPlayer::recordAttackByOpponent(Point /* p */)
{
	// AwfulPlayer completely ignores what the opponent does
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
	bool result(cin >> r >> c);
	if (!result)
		cin.clear();  // clear error state so can do more input operations
	cin.ignore(10000, '\n');
	return result;
}

class HumanPlayer : public Player
{
public:
	HumanPlayer(string nm, const Game& g);
	virtual ~HumanPlayer() {}

	virtual bool isHuman() const { return true; }

	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId) {}
	virtual void recordAttackByOpponent(Point p) {}

};

HumanPlayer::HumanPlayer(string nm, const Game& g)
	:Player(nm, g)
{}

bool HumanPlayer::placeShips(Board& b)
{
	cout << this->name() << " must place 5 ships." << endl;
	b.clear();
	b.display(false);
	bool badInput = false;

	for (int k = 0; k < 5; k++)
	{
		string output;

		switch (k)
		{
		case 0:
			output = "aircraft carrier (length 5):";
			break;
		case 1:
			output = "battleship (length 4):";
			break;
		case 2:
			output = "destroyer (length 3):";
			break;
		case 3:
			output = "submarine (length 3):";
			break;
		case 4:
			output = "patrol ship (length 2):";
			break;
		}

		while (true)
		{
			if (k != 0 && !badInput)
				cin.ignore(100000000, '\n');

			badInput = false;

			string input;
			int row, col;
			cout << "Enter h or v for direction of " << output;
			getline(cin, input);

			if (input[0] == 'h')
			{
				while (true)
				{
					cout << "Enter row and column of leftmost cell (e.g. 3 5):";
					cin >> row >> col;
					if (cin.fail())
					{
						cin.clear();
						cin.ignore();
						cout << "You must input two integers." << endl;
						continue;
					}

					Point p(row, col);
					if (!b.placeShip(p, k, HORIZONTAL))
					{
						cout << "The ship cannot be placed there" << endl;
						continue;
					}
					break;
				}
				break;
			}

			else if (input[0] == 'v')
			{
				while (true)
				{
					cout << "Enter row and column of topmost cell (e.g. 3 5):";
					cin >> row >> col;
					if (cin.fail())
					{
						cin.clear();
						cin.ignore();
						cout << "You must input two integers." << endl;
						continue;
					}
					Point p(row, col);
					if (!b.placeShip(p, k, VERTICAL))
					{
						cout << "The ship cannot be placed there" << endl;
						badInput = true;
						continue;
					}

					break;
				}
				break;
			}

			else
			{
				cout << "Direction must be h or v." << endl;
				continue;
			}
		}

		if (k != 4)
			b.display(false);
	}

	cin.ignore();

	return true;
}

Point HumanPlayer::recommendAttack()
{

	int row, col;

	while (true)
	{
		cout << "Enter the row and column to attack (e.g, 3 5):";
		cin >> row >> col;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore();
			cout << "You must input two integers." << endl;
			continue;
		}

		Point p(row, col);

		return p;
	}

	Point p(0, 0);

	return p;
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer : public Player
{
public:
	MediocrePlayer(string nm, const Game& g);
	virtual ~MediocrePlayer() {}

	virtual bool isHuman() const { return false; }
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p) {}

private:
	bool doesPlace(int shipId, Board& b);
	bool didFire(const Point& p) const;
	bool inBound(const Point& p) const;
	bool inStateOne;

	Point attackResults[100];
	Point currentPoint;
	int attacks;

};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
	:Player(nm, g), inStateOne(true), attacks(0)
{}

///////////////////////////////////////////
//              Helper Functions
///////////////////////////////////////////

//	This function checks wheather the 
//	given point falls into the attack range or not

bool MediocrePlayer::inBound(const Point& p) const
{
	if (p.c == currentPoint.c)
	{
		const int vertical = p.r - currentPoint.r;
		if (vertical <= 4 && vertical >= -4)
			return true;
	}

	if (p.r == currentPoint.r)
	{
		const int horizontal = p.c - currentPoint.c;
		if (horizontal <= 4 && horizontal >= -4)
			return true;
	}

	return false;
}

//	This helper function determines whether the give point has been fired or not

bool MediocrePlayer::didFire(const Point& p) const
{
	for (int k = 0; k < attacks; k++)
		if (p.r == attackResults[k].r && p.c == attackResults[k].c)
			return true;

	return false;
}

//	This helper function places all ships in a recursive manner

bool MediocrePlayer::doesPlace(int shipId, Board& b)
{
	if (shipId < 0)
		return true;

	const int rows = game().rows();
	const int cols = game().cols();

	const int newId = shipId - 1;

	for (int k = 0; k < rows; k++)
	{
		for (int j = 0; j < cols; j++)
		{
			Point p(k, j);

			if (b.placeShip(p, shipId, HORIZONTAL))
			{
				if (!doesPlace(newId, b))
					b.unplaceShip(p, shipId, HORIZONTAL);
				else
					return true;
			}

			if (b.placeShip(p, shipId, VERTICAL))
			{
				if (!doesPlace(newId, b))
					b.unplaceShip(p, shipId, VERTICAL);
				else
					return true;
			}
		}
	}

	return false;
}

///////////////////////////////////////////
//     Public Interface Implementation
///////////////////////////////////////////

bool MediocrePlayer::placeShips(Board& b)
{
	const int shipId = game().nShips() - 1;

	for (int k = 0; k < 50; k++)
	{
		b.block();

		if (doesPlace(shipId, b))
		{
			b.unblock();
			return true;
		}

		b.unblock();
	}

	return false;
}


Point MediocrePlayer::recommendAttack()
{
	if (inStateOne)
	{
		while (true)
		{
			Point p = game().randomPoint();
			if (!didFire(p))
				return p;
		}
	}
	else
	{
		while (true)
		{
			Point p = game().randomPoint();
			if (!didFire(p) && inBound(p))
				return p;
		}
	}
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
	bool shipDestroyed, int shipId)
{
	if (!validShot)
		return;

	attackResults[attacks] = p;
	attacks++;

	if (shotHit && !shipDestroyed && inStateOne)
	{
		inStateOne = false;
		currentPoint = p;
	}

	else if (shotHit && shipDestroyed)
		inStateOne = true;

	return;
}

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer : public Player
{
public:
	GoodPlayer(string nm, const Game& g);
	virtual ~GoodPlayer() {}

	virtual bool isHuman() const { return false; }
	virtual bool placeShips(Board& b);
	virtual Point recommendAttack();
	virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
		bool shipDestroyed, int shipId);
	virtual void recordAttackByOpponent(Point p);

private:
	Point attackResults[100];
	Point hitResults[100];
	Point currentPoint;

	int limit;
	int attacks;
	int hits;

	bool inStateOne;
	bool awefulPlayer;
	bool lastAction;
	bool escape;
	bool didFire(const Point& p) const;
	bool didHit(const Point& p) const;
	bool isUnique(const Point& p) const;
	bool isNear(const Point& p) const;
	bool isNext(const Point& p) const;
	bool isHorizontal() const;
	bool isVertical() const;
	bool inBound(const Point& p);
	bool inBound(const Point& p, Direction dir);
};

GoodPlayer::GoodPlayer(string nm, const Game& g)
	:Player(nm, g), attacks(0), hits(0), limit(1),
	inStateOne(true), awefulPlayer(false), lastAction(false),
	escape(false)
{}

///////////////////////////////////////////
//              Helper Functions
///////////////////////////////////////////

//This helper function determines whether a point
//has been fired or not

bool GoodPlayer::didFire(const Point& p) const
{
	for (int k = 0; k < attacks; k++)
		if (p.r == attackResults[k].r && p.c == attackResults[k].c)
			return true;

	return false;
}

//This helper function determines whether a ship
//has been hit in the point or not

bool GoodPlayer::didHit(const Point& p) const
{
	for (int k = 0; k < hits; k++)
		if (p.r == hitResults[k].r && p.c == hitResults[k].c)
			return true;

	return false;
}

//This helper function determines whether a point's
//coordinates are both odd or both even. This is a 
//useful standard in attacking under state 1. However
//there are special cases involved. Following two helper
//functions help dignose those special cases

bool GoodPlayer::isUnique(const Point& p) const
{
	const int row = p.r;
	const int col = p.c;

	if (row % 2 != 0)
	{
		if (col % 2 != 0)
			return true;
	}

	else
	{
		if (col % 2 == 0)
			return true;
	}

	return false;
}

//This is one of the helper function that I used 
//in attacking under state 1 in order to avoid
//special cases

bool GoodPlayer::isNear(const Point& p) const
{
	Point p1(p.r, p.c + 1);
	Point p2(p.r, p.c - 1);
	Point p3(p.r + 1, p.c);
	Point p4(p.r - 1, p.c);

	if (didHit(p1) || didHit(p2) || didHit(p3) || didHit(p4))
		return true;

	return false;
}

//Same as above

bool GoodPlayer::isNext(const Point& p) const
{
	Point p11(p.r, p.c + 1);
	Point p12(p.r, p.c + 2);
	Point p21(p.r, p.c - 1);
	Point p22(p.r, p.c - 2);
	Point p31(p.r + 1, p.c);
	Point p32(p.r + 2, p.c);
	Point p41(p.r - 1, p.c);
	Point p42(p.r - 2, p.c);

	if ((didHit(p11) && didHit(p12)) || (didHit(p21) && didHit(p22)) ||
		(didHit(p31) && didHit(p32)) || (didHit(p41) && didHit(p42)))
		return true;

	return false;
}

//This function checks whether the point given
//is within the range away from the first hit 
//point regardless of direction.

bool GoodPlayer::inBound(const Point& p)
{
	if (limit > 4)
		limit = 4;

	if (p.c == currentPoint.c)
	{
		const int vertical = p.r - currentPoint.r;
		if (vertical <= limit && vertical >= -limit)
			return true;
	}

	if (p.r == currentPoint.r)
	{
		const int horizontal = p.c - currentPoint.c;
		if (horizontal <= limit && horizontal >= -limit)
			return true;
	}

	return false;
}

//This function checks if the point is within the
//range of attack from the first hit point, taking
//directions into account.

bool GoodPlayer::inBound(const Point& p, Direction dir)
{
	if (limit > 4)
		limit = 4;

	if (dir == HORIZONTAL)
	{
		if (p.r == currentPoint.r)
		{
			const int horizontal = p.c - currentPoint.c;
			if (horizontal <= limit && horizontal >= -limit)
				return true;
		}
	}

	if (dir == VERTICAL)
	{
		if (p.c == currentPoint.c)
		{
			const int vertical = p.r - currentPoint.r;
			if (vertical <= limit && vertical >= -limit)
				return true;
		}
	}

	return false;
}

//This helper function checks if the opponent's
//ship is placed horizontally.

bool GoodPlayer::isHorizontal() const
{
	Point p1(currentPoint.r, currentPoint.c - 1);
	Point p2(currentPoint.r, currentPoint.c + 1);
	if (didHit(p1) || didHit(p2))
		return true;

	return false;
}

//This helper function checks if the opponent's
//ship is placed vertically.

bool GoodPlayer::isVertical() const
{
	Point p1(currentPoint.r - 1, currentPoint.c);
	Point p2(currentPoint.r + 1, currentPoint.c);
	if (didHit(p1) || didHit(p2))
		return true;

	return false;
}

///////////////////////////////////////////
//    Public Interface Implementation
///////////////////////////////////////////

bool GoodPlayer::placeShips(Board& b)
{
	const int nShips = game().nShips();

	for (int k = 0; k < nShips; k++)
	{
		while (true)
		{
			Point p = game().randomPoint();
			const int a = rand() % 2;

			//Let the directin of placement to 
			//be determined by randomness

			if (a == 0)
			{
				if (b.placeShip(p, k, HORIZONTAL))
					break;
				else
					continue;
			}

			else
			{
				if (b.placeShip(p, k, VERTICAL))
					break;
				else
					continue;
			}
		}
	}

	return true;
}

Point GoodPlayer::recommendAttack()
{
	if (inStateOne)
	{
		if (!escape)
		{
			int count = 0;
			while (true)
			{
				count++;
				Point p = game().randomPoint();

				//The following if statement cosiders one of
				//the special cases

				if (!didFire(p) && isUnique(p) || !didFire(p) && isNext(p))
					return p;

				//If the function could not find a suitable
				//point for attack after 120 trials, there
				//must be a speical case

				if (count == 120)
				{
					escape = true;
					break;
				}
			}
		}

		//This deals with the special case

		while (true)
		{
			Point p = game().randomPoint();
			if (!didFire(p) && isUnique(p) || !didFire(p) && isNear(p))
				return p;
		}

	}

	else
	{
		if (!lastAction)
		{
			if (isHorizontal())
			{
				int count = 0;
				while (true)
				{
					count++;
					Point p = game().randomPoint();
					if (!didFire(p) && inBound(p, HORIZONTAL))
						return p;
					if (count == 50)
					{
						lastAction = true;
						break;
					}
				}
			}

			if (isVertical())
			{
				int count = 0;
				while (true)
				{
					count++;
					Point p = game().randomPoint();
					if (!didFire(p) && inBound(p, VERTICAL))
						return p;
					if (count == 50)
					{
						lastAction = true;
						break;
					}
				}
			}
		}

		int count = 0;
		while (true)
		{
			count++;
			Point p = game().randomPoint();
			if (!didFire(p) && inBound(p))
				return p;
			if (count == 60)
			{
				count = 0;
				limit++;
			}
		}
	}
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit,
	bool shipDestroyed, int shipId)
{
	if (!validShot)
		return;

	attackResults[attacks] = p;
	attacks++;

	if (shotHit && !shipDestroyed && inStateOne)
	{
		inStateOne = false;
		currentPoint = p;
		hitResults[hits] = p;
		hits++;
	}

	else if (shotHit && !shipDestroyed && !inStateOne)
	{
		limit++;
		hitResults[hits] = p;
		hits++;
	}

	else if (shotHit && shipDestroyed)
	{
		hitResults[hits] = p;
		hits++;
		inStateOne = true;
		lastAction = false;
		limit = 1;
	}

	return;
}

//This function does nothing

void GoodPlayer::recordAttackByOpponent(Point p)
{

}



//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
	static string types[] = {
		"human", "awful", "mediocre", "good"
	};

	int pos;
	for (pos = 0; pos != sizeof(types) / sizeof(types[0]) &&
		type != types[pos]; pos++)
		;
	switch (pos)
	{
	case 0:  return new HumanPlayer(nm, g);
	case 1:  return new AwfulPlayer(nm, g);
	case 2:  return new MediocrePlayer(nm, g);
	case 3:  return new GoodPlayer(nm, g);
	default: return nullptr;
	}
}
