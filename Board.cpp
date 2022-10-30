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
      // TODO:  Decide what private members you need.  Here's one that's likely
      //        to be useful:
    const Game& m_game;
    char m_board[MAXROWS][MAXCOLS];
};

BoardImpl::BoardImpl(const Game& g)
 : m_game(g)
{
    // This compiles, but may not be correct
    for (int i = 0; i < g.rows(); i++)
        for (int j = 0; j < g.cols(); j++)
            m_board[i][j] = '.';
}

void BoardImpl::clear()
{
    // This compiles, but may not be correct
    for (int i = 0; i < m_game.rows(); i++)
        for (int j = 0; j < m_game.cols(); j++)
            m_board[i][j] = '.';
}

void BoardImpl::block()
{
// TODO: Replace this with code to block half of the cells on the board
    int i = 0;
    while (i < m_game.rows() * m_game.cols() / 2)
    {
        Point temp = m_game.randomPoint();
        if (m_board[temp.r][temp.c] == '#')
            ;
        else if (m_board[temp.r][temp.c] != '#')
        {
            m_board[temp.r][temp.c] = '#';
            i++;
        }
    }
}

void BoardImpl::unblock()
{
// TODO: Replace this with code to unblock all blocked cells
    for (int i = 0; i < m_game.rows(); i++)
        for (int j = 0; j < m_game.cols(); j++)
            if (m_board[i][j] == '#')
                m_board[i][j] = '.';
}
 
bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId > m_game.nShips() || shipId < 0)
        return false;
    if (!m_game.isValid(topOrLeft))
        return false;
    for (int i = 0; i < m_game.cols(); i++)
        for (int j = 0; j < m_game.rows(); j++)
            if (m_board[i][j] == m_game.shipSymbol(shipId))
                return false;
    if (dir == HORIZONTAL)
    {
        if(topOrLeft.c + m_game.shipLength(shipId) > m_game.cols())
            return false;
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            if (m_board[topOrLeft.r][topOrLeft.c + i] != '.')
                return  false;
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r][topOrLeft.c + i] = m_game.shipSymbol(shipId);
    }
    else if (dir == VERTICAL)
    {
        if(topOrLeft.r + m_game.shipLength(shipId) > m_game.rows())
            return false;
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            if (m_board[topOrLeft.r + i][topOrLeft.c] != '.')
                return false;
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r + i][topOrLeft.c] = m_game.shipSymbol(shipId);
    }
    return true; // This compiles, but may not be correct
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    if (shipId > m_game.nShips() || shipId < 0)
        return false;
    if (!m_game.isValid(topOrLeft))
        return false;
    if (dir == HORIZONTAL)
    {
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            if (m_board[topOrLeft.r][topOrLeft.c+i] != m_game.shipSymbol(shipId))
                return false;
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r][topOrLeft.c + i] = '.';
    }
    else if (dir == VERTICAL)
    {
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            if (m_board[topOrLeft.r + i][topOrLeft.c] != m_game.shipSymbol(shipId))
                return false;
        for (int i = 0; i < m_game.shipLength(shipId); i++)
            m_board[topOrLeft.r + i][topOrLeft.c] = '.';
    }
    return true; // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
    cout << "  ";
    for (int i = 0; i < m_game.cols(); i++)
        cout << i;
    cout << endl;
    if (!shotsOnly)
        for (int i = 0; i < m_game.rows(); i++)
        {
            cout << i << " ";
            for (int j = 0; j < m_game.cols(); j++)
                cout << m_board[i][j];
            cout << endl;
        }
    else
        for (int i = 0; i < m_game.rows(); i++)
        {
            cout << i << " ";
            for (int j = 0; j < m_game.cols(); j++){
                if (m_board[i][j] != 'o' && m_board[i][j] != 'X' && m_board[i][j] != '.')
                    cout << '.';
                else
                    cout << m_board[i][j];
            }
            cout << endl;
        }
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    if (p.r > m_game.rows() - 1 || p.r < 0 || p.c < 0 || p.c > m_game.cols())
    {
        shotHit = false;
        return false;
    }
    if (m_board[p.r][p.c] == 'o' || m_board[p.r][p.c] == 'X')
    {
        shotHit = false;
        return false;
    }
    if (m_board[p.r][p.c] != '.')
    {
        shotHit = true;
        bool shipExists = false;
        char check = m_board[p.r][p.c];
        m_board[p.r][p.c] = 'X';
        for (int i = 0; i < m_game.rows(); i++)
            for (int j = 0; j < m_game.cols(); j++)
                if (m_board[i][j] == check)
                {
                    shipExists = true;
                    break;
                }
        if (!shipExists)
        {
            shipDestroyed = true;
            for (int i = 0; i < m_game.nShips(); i++)
                if (check == m_game.shipSymbol(i))
                {
                    shipId = i;
                    break;
                }
        }
    }
    else
    {
        m_board[p.r][p.c] = 'o';
        shotHit = false;
    }
    return true; // This compiles, but may not be correct
}

bool BoardImpl::allShipsDestroyed() const
{
    bool allDestoryed = true;
    for (int i = 0; i < m_game.rows(); i++)
        for (int j = 0; j < m_game.cols(); j++)
            if (m_board[i][j] != '.' && m_board[i][j] != 'o' && m_board[i][j] != 'X')
                allDestoryed = false;
    return allDestoryed; // This compiles, but may not be correct
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
