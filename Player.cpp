#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <stack>
using namespace std;

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
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
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

// TODO:  You need to replace this with a real class declaration and
//        implementation.
//typedef AwfulPlayer HumanPlayer;

class HumanPlayer : public Player
{
public:
    HumanPlayer(string name, const Game& g);
    ~HumanPlayer(){}
    
    bool isHuman() const { return true; }
    bool placeShips(Board& b);
    Point recommendAttack();
    void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId){}
    void recordAttackByOpponent(Point p){}
};
HumanPlayer::HumanPlayer(string name, const Game& g) : Player(name, g){}
bool HumanPlayer::placeShips(Board &b)
{
    int r, c;
    char dir;
    bool isValid = true;
    for (int i = 0; i < game().nShips(); i++){
        //        do {
        do {
            cout << "Enter h or v for direction of " << game().shipName(i) << " (length " << game().shipLength(i) << "): ";
            cin >> dir;
            if (dir != 'h' && dir != 'v')
            {
                cout << "Direction must be h or v." << endl;
                cin.clear();  // clear error state so can do more input operations
                cin.ignore(10000, '\n');
            }
        } while (dir != 'h' && dir != 'v');
        do{
            do{
                if (dir == 'h')
                    cout << "Enter row and column of leftmost cell (e.g., 3 5): ";
                else
                    cout << "Enter row and column of topmost cell (e.g., 3 5): ";
                isValid = getLineWithTwoIntegers(r, c);
                if (!isValid)
                    cout << "You must enter two integers." << endl;
            } while(!isValid);
            if (dir == 'h')
                isValid = b.placeShip(Point(r,c), i, HORIZONTAL);
            else
                isValid = b.placeShip(Point(r,c), i, VERTICAL);
            if (!isValid)
                cout << "The ship can not be placed there." << endl;
        } while(!isValid);
        b.display(false);
    }
    return true;
}
Point HumanPlayer::recommendAttack()
{
    int r, c;
    bool isValid = true;
    do{
        cout << "Enter the row and column to attack (e.g., 3 5): ";
        isValid = getLineWithTwoIntegers(r, c);
        if (!isValid)
            cout << "You must enter two integers." << endl;
    }while (!isValid);
    return Point(r,c);
}

//*********************************************************************
//  MediocrePlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class MediocrePlayer : public Player
{
public:
    MediocrePlayer(string name, const Game& g) : Player(name, g){}
    ~MediocrePlayer(){};
    bool placeShips(Board& b);
    Point recommendAttack();
    void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    void recordAttackByOpponent(Point p){};
private:
    bool state;
    vector<Point> shots;
    vector<Point> possibleShots;
    Point lastHit;
};
//MediocrePlayer::~MediocrePlayer(){}

bool placeAllShips(int numShips, int shipId, Board& b, const Game& g, int count)
{
    bool shipPlaced = false;
    if (numShips > 0 && count <= 50)
    {
        for (int i = 0; i < g.rows(); i++)
        {
            for (int j = 0; j < g.cols(); j++)
            {
                if (b.placeShip(Point(i,j), shipId, VERTICAL))
                {
                    shipPlaced = true;
                    placeAllShips(numShips - 1, shipId+1, b, g, 0);
                }
                else if (b.placeShip(Point(i,j), shipId, HORIZONTAL))
                {
                    shipPlaced = true;
                    placeAllShips(numShips - 1, shipId + 1, b, g, 0);
                }
            }
        }
    }
    return shipPlaced;
}
bool MediocrePlayer::placeShips(Board &b)
{
    bool placeAll;
    int count = 0;
label:
    {
        b.block();
        placeAll = placeAllShips(game().nShips(), 0, b, game(), 0);
        b.unblock();
    }
    if (!placeAll && count < 50){
        count++;
        b.clear();
        goto label;
    }
    return placeAll;
}

Point MediocrePlayer::recommendAttack()
{
    Point attack;
    bool valid = true;
    if (!state)
    {
    label:
        {
            attack = game().randomPoint();
            for (int i = 0; i < shots.size(); i++)
                if (attack.r == shots[i].r && attack.c == shots[i].c)
                {
                    valid = false;
                    break;
                }
                else
                    valid = true;
        } if (!valid)
            goto label;
        shots.push_back(attack);
    }
    else if (state)
    {
        if(possibleShots.empty())
        {
        tryAgain:
            {
                int r = rand()%4;
                switch (r) {
                    case 0:
                        if (lastHit.r + 4 < game().rows())
                        {
                            int c = rand()% 4+1;
                            attack = Point(lastHit.r+c, lastHit.c);
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                        else
                        {
                            if (lastHit.r == game().rows()-1)
                                goto tryAgain;
                            else
                            {
                                int j = game().rows() - lastHit.r - 1;
                                int c = rand() % j + 1;
                                attack = Point(lastHit.r+c, lastHit.c);
                                possibleShots.push_back(attack);
                                shots.push_back(attack);
                            }
                        }
                        break;
                    case 1:
                        if (lastHit.r - 4 >= 0)
                        {
                            int c = rand()% 4+1;
                            attack = Point(lastHit.r-c, lastHit.c);
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                        else
                        {
                            if (lastHit.r == 0)
                                goto tryAgain;
                            else
                            {
                                int c = rand() % lastHit.r + 1;
                                attack = Point(lastHit.r - c, lastHit.c);
                                possibleShots.push_back(attack);
                                shots.push_back(attack);
                            }
                        }
                        break;
                    case 2:
                        if (lastHit.c + 4 < game().cols())
                        {
                            int c = rand()% 4+1;
                            attack = Point(lastHit.r, lastHit.c+c);
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                        else
                        {
                            if (lastHit.c == game().cols()-1)
                                goto tryAgain;
                            else
                            {
                                int j = game().cols() - lastHit.c - 1;
                                int c = rand() % j + 1;
                                attack = Point(lastHit.r, lastHit.c+c);
                                possibleShots.push_back(attack);
                                shots.push_back(attack);
                            }
                        }
                        break;
                    case 3:
                        if (lastHit.c - 4 >= 0)
                        {
                            int c = rand()% 4+1;
                            attack = Point(lastHit.r, lastHit.c-c);
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                        else
                        {
                            if (lastHit.r == 0)
                                goto tryAgain;
                            else
                            {
                                int c = rand() % lastHit.r + 1;
                                attack = Point(lastHit.r - c, lastHit.c);
                                possibleShots.push_back(attack);
                                shots.push_back(attack);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
        tryAgain1:
            int r = rand()%4;
            switch (r) {
                case 0:
                    if (lastHit.r + 4 < game().rows())
                    {
                        int c = rand()% 4+1;
                        attack = Point(lastHit.r+c, lastHit.c);
                        for(int i = 0; i < possibleShots.size(); i++)
                            if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                goto tryAgain1;
                        possibleShots.push_back(attack);
                        shots.push_back(attack);
                    }
                    else
                    {
                        if (lastHit.r == game().rows()-1)
                            goto tryAgain;
                        else
                        {
                            int j = game().rows() - lastHit.r - 1;
                            int c = rand() % j + 1;
                            attack = Point(lastHit.r+c, lastHit.c);
                            for(int i = 0; i < possibleShots.size(); i++)
                                if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                    goto tryAgain1;
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                    }
                    break;
                case 1:
                    if (lastHit.r - 4 >= 0)
                    {
                        int c = rand()% 4+1;
                        attack = Point(lastHit.r-c, lastHit.c);
                        for(int i = 0; i < possibleShots.size(); i++)
                            if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                goto tryAgain1;
                        possibleShots.push_back(attack);
                        shots.push_back(attack);
                    }
                    else
                    {
                        if (lastHit.r == 0)
                            goto tryAgain1;
                        else
                        {
                            int c = rand() % lastHit.r + 1;
                            attack = Point(lastHit.r - c, lastHit.c);
                            for(int i = 0; i < possibleShots.size(); i++)
                                if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                    goto tryAgain1;
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                    }
                    break;
                case 2:
                    if (lastHit.c + 4 < game().cols())
                    {
                        int c = rand()% 4+1;
                        attack = Point(lastHit.r, lastHit.c+c);
                        for(int i = 0; i < possibleShots.size(); i++)
                            if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                goto tryAgain1;
                        possibleShots.push_back(attack);
                        shots.push_back(attack);
                    }
                    else
                    {
                        if (lastHit.c == game().cols()-1)
                            goto tryAgain;
                        else
                        {
                            int j = game().cols() - lastHit.c - 1;
                            int c = rand() % j + 1;
                            attack = Point(lastHit.r, lastHit.c+c);
                            for(int i = 0; i < possibleShots.size(); i++)
                                if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                    goto tryAgain1;
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                    }
                    break;
                case 3:
                    if (lastHit.c - 4 >= 0)
                    {
                        int c = rand()% 4+1;
                        attack = Point(lastHit.r, lastHit.c-c);
                        for(int i = 0; i < possibleShots.size(); i++)
                            if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                goto tryAgain1;
                        possibleShots.push_back(attack);
                        shots.push_back(attack);
                    }
                    else
                    {
                        if (lastHit.c == 0)
                            goto tryAgain1;
                        else
                        {
                            int c = rand() % lastHit.c + 1;
                            attack = Point(lastHit.r, lastHit.c-c);
                            for(int i = 0; i < possibleShots.size(); i++)
                                if (attack.r == possibleShots[i].r && attack.c == possibleShots[i].c)
                                    goto tryAgain1;
                            possibleShots.push_back(attack);
                            shots.push_back(attack);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    return attack;
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    if (shotHit && possibleShots.empty())
    {
        state = true;
        lastHit = p;
    }
    else if(shotHit && !shipDestroyed)
    {
        int right, left, down, up;
        if (lastHit.r + 4 < game().rows())
            down = 4;
        else
            down = game().rows() - lastHit.r - 1;
        if (lastHit.r - 4 >= 0)
            up = 4;
        else
            up = lastHit.r;
        if (lastHit.c + 4 < game().cols())
            right = 4;
        else
            right = game().cols() - lastHit.c - 1;
        if (lastHit.c - 4 >= 0)
            left = 4;
        else
            left = lastHit.c;
        if (possibleShots.size() == left + right + down + up)
        {
            state = false;
            possibleShots.clear();
        }
    }
    else if(shotHit && shipDestroyed)
    {
        state = false;
        possibleShots.clear();
    }
    else if (!shotHit && state)
    {
        int right, left, down, up;
        if (lastHit.r + 4 < game().rows())
            down = 4;
        else
            down = game().rows() - lastHit.r - 1;
        if (lastHit.r - 4 >= 0)
            up = 4;
        else
            up = lastHit.r;
        if (lastHit.c + 4 < game().cols())
            right = 4;
        else
            right = game().cols() - lastHit.c - 1;
        if (lastHit.c - 4 >= 0)
            left = 4;
        else
            left = lastHit.c;
        if (possibleShots.size() == left + right + down + up)
        {
            state = false;
            possibleShots.clear();
        }
    }
}


// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

// TODO:  You need to replace this with a real class declaration and
//        implementation.
class GoodPlayer : public Player
{
public:
    GoodPlayer(string name, const Game& g) : Player(name, g){}
    ~GoodPlayer(){};
    bool placeShips(Board& b);
    Point recommendAttack();
    void recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId);
    void recordAttackByOpponent(Point p){};
private:
    char Direction;
    bool firstHit = false, contHit = false;
    bool shipsDest = false;
    int state = 0;
    vector<Point> shots;    Point lasthit, hitb4change;
};

bool GoodPlayer::placeShips(Board &b)
{
    bool placeAll;
    if (game().rows() != 10 && game().cols() != 10 && game().nShips() != 5)
    {
        int count = 0;
    label:
        {
            b.block();
            placeAll = placeAllShips(game().nShips(), 0, b, game(), 0);
            b.unblock();
        }
        if (!placeAll && count < 50){
            count++;
            b.clear();
            goto label;
        }
    }
    else
    {
        placeAll = b.placeShip(Point(9,4), 0, HORIZONTAL);
        placeAll = b.placeShip(Point(0,0), 1, VERTICAL);
        placeAll = b.placeShip(Point(2,6), 2, HORIZONTAL);
        placeAll = b.placeShip(Point(1,3), 3, HORIZONTAL);
        placeAll = b.placeShip(Point(4,5), 4, VERTICAL);
    }
    return placeAll;
}



Point GoodPlayer::recommendAttack()
{
    Point attack;
    bool valid = true;
    switch (state) {
        case 0:
        {
            attack = Point(0,0);
            bool move = false;
            for (int i = 0; i < shots.size(); i++)
            {
                if (shots[i].r == 9 && shots[i].c == 9)
                {
                    for (int j = 0; j < shots.size(); j++)
                    {
                        if (attack.r == shots[i].r && attack.c == shots[i].c)
                        {
                            if (attack.c == game().cols()-1)
                            {
                                attack.c = 0;
                                attack.r += 1;
                            }
                            else
                                attack.c += 1;
                        }
                        else
                        {
                            move = true;
                            break;
                        }
                    }
                }
            }
            if (move)
                break;
            if (shots.empty())
            {
                attack.r = 0;
                attack.c = 0;
            }
            else if (shipsDest)
            {
                Point check = hitb4change;
                if (hitb4change.r % 2 == 0)
                {
                check1:
                    for (int i = 0; i < shots.size(); i++)
                    {
                        
                        if (shots[i].r == check.r && check.c == shots[i].c)
                        {
                            if (check.c == game().cols()-2)
                            {
                                if (check.r % 2 == 0)
                                {
                                    check.c = 1;
                                    check.r += 1;
                                }
                                else
                                {
                                    check.c = 0;
                                    check.r += 1;
                                }
                            }
                            else
                                check.c += 2;
                            goto check1;
                        }
                    }
                    attack.r = check.r;
                    attack.c = check.c;
                }
                else
                {
                check2:
                    for (int i = 0; i < shots.size(); i++)
                    {
                        
                        if (shots[i].r == check.r && check.c == shots[i].c)
                        {
                            if (check.c == game().cols()-1)
                            {
                                if (check.r %  2 == 0)
                                {
                                    check.c = 1;
                                    check.r += 1;
                                }
                                else
                                {
                                    check.c = 0;
                                    check.r += 1;
                                }
                            }
                            else
                                check.c += 2;
                            goto check2;
                        }
                    }
                    attack.r = check.r;
                    attack.c = check.c;
                }
                shipsDest = false;
            }
            else if (shots.back().r % 2 == 0)
            {
            check3:
                for (int i = 0; i < shots.size(); i++)
                {
                    if (attack.r == shots[i].r && attack.c == shots[i].c)
                    {
                        if (attack.r % 2 == 0)
                        {
                            if (attack.c + 2 <= game().cols()-1)
                                attack.c += 2;
                            else
                            {
                                attack.c = 1;
                                attack.r += 1;
                            }
                        }
                        else
                        {
                            if (attack.c + 2 <= game().cols()-1)
                                attack.c += 2;
                            else
                            {
                                attack.c = 0;
                                attack.r += 1;
                            }
                        }
                        goto check3;
                    }
                }
                
            }
            else if (shots.back().r % 2 != 0)
            {
            check4:
                for (int i = 0; i < shots.size(); i++)
                {
                    if (attack.r == shots[i].r && attack.c == shots[i].c)
                    {
                        if (attack.r % 2 == 0)
                        {
                            if (attack.c + 2 <= game().cols()-1)
                                attack.c += 2;
                            else
                            {
                                attack.c = 1;
                                attack.r += 1;
                            }
                        }
                        else
                        {
                            if (attack.c + 2 <= game().cols()-1)
                                attack.c += 2;
                            else
                            {
                                attack.c = 0;
                                attack.r += 1;
                            }
                        }
                        goto check4;
                    }
                }
            }
            break;
        }
        case 1:
        {
            int counter = 0;
        TryAgain:
            {
                if (counter <= 1000)
                {
                    int c = rand()%4;
                    switch (c) {
                        case 0:
                            if (hitb4change.r == game().rows()-1)
                                goto TryAgain;
                            for (int i = 0;  i < shots.size(); i++)
                                if (hitb4change.r+1 == shots[i].r && hitb4change.c == shots[i].c)
                                {
                                        counter++;
                                        goto TryAgain;
                                }
                            attack = Point(hitb4change.r+1, hitb4change.c);
                            break;
                        case 1:
                            if (hitb4change.r == 0)
                                goto TryAgain;
                            for (int i = 0;  i < shots.size(); i++)
                                if (hitb4change.r-1 == shots[i].r && hitb4change.c == shots[i].c)
                                    if (hitb4change.r+1 == shots[i].r && hitb4change.c == shots[i].c)
                                    {
                                            counter++;
                                            goto TryAgain;
                                    }
                            attack = Point(hitb4change.r-1, hitb4change.c);
                            break;
                        case 2:
                            if (hitb4change.c == game().cols()-1)
                                goto TryAgain;
                            for (int i = 0;  i < shots.size(); i++)
                                if (hitb4change.r == shots[i].r && hitb4change.c+1 == shots[i].c)
                                    if (hitb4change.r+1 == shots[i].r && hitb4change.c == shots[i].c)
                                    {
                                            counter++;
                                            goto TryAgain;
                                    }
                            attack = Point(hitb4change.r, hitb4change.c+1);
                            break;
                        case 3:
                            if (lasthit.c == 0)
                                goto TryAgain;
                            for (int i = 0;  i < shots.size(); i++)
                                if (lasthit.r == shots[i].r && lasthit.c-1 == shots[i].c)
                                    if (hitb4change.r+1 == shots[i].r && hitb4change.c == shots[i].c)
                                    {
                                            counter++;
                                            goto TryAgain;
                                    }
                            attack = Point(lasthit.r, lasthit.c-1);
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    attack = Point(0,0);
                    for (int j = 0; j < shots.size(); j++)
                    {
                        if (attack.r == shots[j].r && attack.c == shots[j].c)
                        {
                            if (attack.c == game().cols()-1)
                            {
                                attack.c = 0;
                                attack.r += 1;
                            }
                            else
                                attack.c += 1;
                        }
                        else
                            break;
                    }
                }
            }
            break;
        }
        case 2:
        {
            switch (Direction) {
                case 's':
                    for (int i = 0; i < shots.size(); i++)
                        if ((lasthit.r+1 == shots[i].r && lasthit.c == shots[i].c) || lasthit.r + 1 >= game().rows())
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                        attack = Point(lasthit.r + 1, lasthit.c);
                    break;
                case 'w':
                    valid = true;
                    for (int i = 0; i < shots.size(); i++)
                        if ((lasthit.r-1 == shots[i].r && lasthit.c == shots[i].c) || lasthit.r - 1 < 0)
                        {
                            valid = false;
                            break;
                        }
                    if(valid)
                        attack = Point(lasthit.r - 1, lasthit.c);
                    break;
                case 'd':valid = true;
                    for (int i = 0; i < shots.size(); i++)
                        if ((lasthit.r == shots[i].r && lasthit.c + 1 == shots[i].c) || lasthit.c + 1 >= game().cols())
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                        attack = Point(lasthit.r, lasthit.c + 1);
                    break;
                case 'a':
                    valid = true;
                    for (int i = 0; i < shots.size(); i++)
                        if ((lasthit.r == shots[i].r && lasthit.c - 1 == shots[i].c) || lasthit.c - 1 < 0)
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                        attack = Point(lasthit.r, lasthit.c - 1);
                    break;
                default:
                    break;
            }
            break;
        }
    case 3:
        {
            switch (Direction) {
                case 's':
                    valid = true;
                    attack = Point(hitb4change.r - 1, hitb4change.c);
                    for (int i = 0; i < shots.size(); i++)
                        if ((attack.r == shots[i].r && attack.c == shots[i].c) || attack.r <0)
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        Direction = 'w';
                        state = 2;
                        firstHit = true;
                        break;
                    }
                case 'w':
                    valid = true;
                    attack = Point(hitb4change.r + 1, hitb4change.c);
                    for (int i = 0; i < shots.size(); i++)
                        if ((attack.r == shots[i].r && attack.c == shots[i].c) || attack.r == game().rows())
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        Direction = 's';
                        state = 2;
                        firstHit = true;
                        break;
                    }
                case 'd':
                    valid = true;
                    attack = Point(hitb4change.r, hitb4change.c - 1);
                    for (int i = 0; i < shots.size(); i++)
                        if ((attack.r == shots[i].r && attack.c == shots[i].c) || attack.c < 0)
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        Direction = 'a';
                        state = 2;
                        firstHit = true;
                        break;
                    }
                case 'a':
                    valid = true;
                    attack = Point(hitb4change.r, hitb4change.c + 1);
                    for (int i = 0; i < shots.size(); i++)
                        if ((attack.r == shots[i].r && attack.c == shots[i].c) || attack.r == game().cols())
                        {
                            valid = false;
                            break;
                        }
                    if (valid)
                    {
                        Direction = 'd';
                        state = 2;
                        firstHit = true;
                        break;
                    }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    shots.push_back(attack);
    return attack;
}

void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
    Point attack = Point(0,0);
    bool move = true;
    for (int i = 0; i < shots.size(); i++)
        if (shots[i].r == 9 && shots[i].c == 9)
            move = false;
    if (!move)
        state = 0;
    else if (shotHit && !shipDestroyed && !firstHit)
    {
        state = 1;
        lasthit = p;
        firstHit = true;
        shipsDest = false;
        if (!contHit)
            hitb4change = p;
    }
    else if (shotHit && !shipDestroyed && firstHit)
    {
        state = 2;
        lasthit = p;
        if (p.r == hitb4change.r + 1)
            Direction = 's';
        else if (p.r == hitb4change.r - 1)
            Direction = 'w';
        else if (p.c == hitb4change.c + 1)
            Direction = 'd';
        else if (p.c == hitb4change.c - 1)
            Direction = 'a';
        contHit = true;
    }
    else if (!shotHit && !shipDestroyed && firstHit && contHit)
    {
        state = 3;
        firstHit = false;
    }
    else if (shotHit && shipDestroyed)
    {
        state = 0;
        firstHit = false;
        shipsDest = true;
        contHit = false;
    }
    else if (!shotHit && !firstHit)
    {
        state = 0;
    }
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
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
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
