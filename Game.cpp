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
    int m_rows;
    int m_cols;
    int m_numShips = 0;
    struct Ship
    {
        Ship(int shipID, int length, char symbol, string name):s_ID(shipID), s_length(length), s_symbol(symbol), s_name(name){}
        int s_ID, s_length;
        char s_symbol;
        string s_name;
    };
    vector<Ship> m_ships;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols) : m_rows(nRows), m_cols(nCols){}

int GameImpl::rows() const
{
    return m_rows;  // This compiles but may not be correct
}

int GameImpl::cols() const
{
    return m_cols;  // This compiles but may not be correct
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
    m_ships.push_back(Ship(m_numShips, length, symbol, name));
    m_numShips++;
    return true;  // This compiles but may not be correct
}

int GameImpl::nShips() const
{
    return m_numShips;  // This compiles but may not be correct
}

int GameImpl::shipLength(int shipId) const
{
    return m_ships[shipId].s_length;  // This compiles but may not be correct
}

char GameImpl::shipSymbol(int shipId) const
{
    return m_ships[shipId].s_symbol;  // This compiles but may not be correct
}

string GameImpl::shipName(int shipId) const
{
    return m_ships[shipId].s_name;  // This compiles but may not be correct
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
    Point attack;
    bool shipHit, shipDestroyed = false, validShot;
    int shipID;
    if (!p1->isHuman() && !p2->isHuman())
    {
        bool valid1 = p1->placeShips(b1), valid2 = p2->placeShips(b2);
        if (!valid1 || !valid2)
            return nullptr;
        while(!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
        {
            cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << endl;
            b2.display(false);
            attack = p1->recommendAttack();
            validShot = b2.attack(attack, shipHit, shipDestroyed, shipID);
            p1->recordAttackResult(attack, validShot, shipHit, shipDestroyed, shipID);
            p2->recordAttackByOpponent(attack);
            shipDestroyed = false;
            if (!shipHit)
            {
                cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
                b2.display(false);
            }
            else
            {
                if (!shipDestroyed){
                    cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
                    b2.display(false);
                }
                else
                {
                    cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipID) << ", resulting in:" << endl;
                    b2.display(false);
                }
            }
            if (b1.allShipsDestroyed())
            {
                cout << p2->name() << " wins!" << endl;
                cout << endl;
                return p2;
                break;
            }
            else if (b2.allShipsDestroyed())
            {
                cout << p1->name() << " wins!" << endl;
                return p1;
                break;
            }
            if (shouldPause)
            {
                cout << "Press enter to continue: ";
                cin.ignore(100000, '\n');
            }
            cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
            b1.display(false);
            attack = p2->recommendAttack();
            validShot = b1.attack(attack, shipHit, shipDestroyed, shipID);
            p2->recordAttackResult(attack, validShot, shipHit, shipDestroyed, shipID);
            p1->recordAttackByOpponent(attack);
            shipDestroyed = false;
            if (!shipHit)
            {
                cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
                b1.display(false);
            }
            else
            {
                if (!shipDestroyed){
                    cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
                    b1.display(false);
                }
                else
                {
                    cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipID) << ", resulting in:" << endl;
                    b1.display(false);
                }
            }
            if (b1.allShipsDestroyed())
            {
                cout << p2->name() << " wins!" << endl;
                cout << endl;
                return p2;
                break;
            }
            else if (b2.allShipsDestroyed())
            {
                cout << p1->name() << " wins!" << endl;
                return p1;
                break;
            }
            if (shouldPause)
            {
                cout << "Press enter to continue: ";
                cin.ignore(100000, '\n');
            }
        }
    
    }
    else if (p1->isHuman())
    {
        cout << p1->name() << " must place 5 ships." << endl;
        b1.display(false);
        if(!p2->placeShips(b2))
            return nullptr;
        p1->placeShips(b1);
        while(!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
        {
            cout << p1->name() << "'s turn. Board for " << p2->name() << ":" << endl;
            b2.display(true);
            attack = p1->recommendAttack();
            if(!b2.attack(attack, shipHit, shipDestroyed, shipID))
                cout << p1->name() << " wassted a shot at (" << attack.r << "," << attack.c << ")." << endl;
            else
            {
                if (!shipHit)
                {
                    cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
                    b2.display(true);
                }
                else
                {
                    if (!shipDestroyed){
                        cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
                        b2.display(true);
                    }
                    else
                    {
                        cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipID) << ", resulting in:" << endl;
                        b2.display(true);
                    }
                }
            }
            if (b1.allShipsDestroyed())
            {
                cout << p2->name() << " wins!" << endl;
                b2.display(false);
                cout << endl;
                return p2;
                break;
            }
            else if (b2.allShipsDestroyed())
            {
                cout << p1->name() << " wins!" << endl;
                return p1;
                break;
            }
            p2->recordAttackByOpponent(attack);
            if (shouldPause)
            {
                cout << "Press enter to continue: ";
                cin.ignore(100000, '\n');
            }
            cout << p2->name() << "'s turn.  Board for " << p1->name() << ":" << endl;
            b1.display(false);
            attack = p2->recommendAttack();
            validShot = b1.attack(attack, shipHit, shipDestroyed, shipID);
            p2->recordAttackResult(attack, validShot, shipHit, shipDestroyed, shipID);
            shipDestroyed = false;
            if (!shipHit)
            {
                cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
                b1.display(false);
            }
            else
            {
                if (!shipDestroyed){
                    cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
                    b1.display(false);
                }
                else
                {
                    cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipID) << ", resulting in:" << endl;
                    b1.display(false);
                }
            }
            if (shouldPause)
                    {
                        cout << "Press enter to continue: ";
                        cin.ignore(100000, '\n');
                    }
        }
        if (b1.allShipsDestroyed())
        {
            cout << p2->name() << " wins!" << endl;
            b2.display(false);
            cout << endl;
            return p2;
        }
        else
        {
            cout << p1->name() << " wins!" << endl;
            return p1;
        }
    }
    else if(p2->isHuman())
    {
        cout << p2->name() << " must place 5 ships." << endl;
        b2.display(false);
        p2->placeShips(b2);
        if(!p1->placeShips(b1))
            return nullptr;
        while(!b1.allShipsDestroyed() && !b2.allShipsDestroyed())
        {
            cout << p1->name() << "'s turn.  Board for " << p2->name() << ":" << endl;
            b2.display(false);
            attack = p1->recommendAttack();
            validShot = b2.attack(attack, shipHit, shipDestroyed, shipID);
            p1->recordAttackResult(attack, validShot, shipHit, shipDestroyed, shipID);
            if (!shipHit)
            {
                cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
                b2.display(false);
            }
            else
            {
                if (!shipDestroyed){
                    cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
                    b2.display(false);
                }
                else
                {
                    cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipID) << ", resulting in:" << endl;
                    b2.display(false);
                }
            }
            if (b1.allShipsDestroyed())
            {
                cout << p2->name() << " wins!" << endl;
                cout << endl;
                return p2;
                break;
            }
            else if (b2.allShipsDestroyed())
            {
                cout << p1->name() << " wins!" << endl;
                return p1;
                break;
            }
            if(shouldPause)
            {
                cout << "Press enter to continue: ";
                cin.ignore(100000, '\n');
            }
            cout << p2->name() << "'s turn. Board for " << p1->name() << ":" << endl;
            b1.display(true);
            attack = p2->recommendAttack();
            if(!b1.attack(attack, shipHit, shipDestroyed, shipID))
                cout << p2->name() << " wassted a shot at (" << attack.r << "," << attack.c << ")." << endl;
            else
            {
                if (!shipHit)
                {
                    cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
                    b1.display(true);
                }
                else
                {
                    if (!shipDestroyed){
                        cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
                        b1.display(true);
                    }
                    else
                    {
                        cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipID) << ", resulting in:" << endl;
                        b1.display(true);
                    }
                }
            }
            p1->recordAttackByOpponent(attack);
            shipDestroyed = false;
            if (shouldPause)
            {
                cout << "Press enter to continue: ";
                cin.ignore(100000, '\n');
            }
        }
        if (b1.allShipsDestroyed())
        {
            cout << p2->name() << " wins!" << endl;
            return p2;
        }
        else
        {
            cout << p1->name() << " wins!" << endl;
            b1.display(false);
            cout << endl;
            return p1;
        }
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

