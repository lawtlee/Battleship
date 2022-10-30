#include "Game.h"
#include "Player.h"
#include <iostream>
#include <string>
#include "Board.h"
using namespace std;

bool addStandardShips(Game& g)
{
    return g.addShip(5, 'A', "aircraft carrier")  &&
           g.addShip(4, 'B', "battleship")  &&
           g.addShip(3, 'D', "destroyer")  &&
           g.addShip(3, 'S', "submarine")  &&
           g.addShip(2, 'P', "patrol boat");
}

int main()
{
    const int NTRIALS = 10;
    char cont;
do
{
    cout << "Select one of these choices for an example of the game:" << endl;
    cout << "  1.  A mini-game between two mediocre players" << endl;
    cout << "  2.  A mediocre player against a human player" << endl;
    cout << "  3.  A " << NTRIALS
         << "-game match between a mediocre and an awful player, with no pauses"
         << endl;
    cout << "  4.  A game between human and awful player" << endl;
    cout << "  5.  A game between two awful players" << endl;
    cout << "  6.  A game between a mediocre player and a good player." << endl;
    cout << "Enter your choice: ";
    string line;
    getline(cin,line);
    if (line.empty())
    {
        cout << "You did not enter a choice" << endl;
    }
    else if (line[0] == '1')
    {
//        Game g(2, 3);
//        g.addShip(2, 'R', "rowboat");
        Game g(10,10);
        addStandardShips(g);
        Player* p1 = createPlayer("mediocre", "Popeye", g);
        Player* p2 = createPlayer("mediocre", "Bluto", g);
        cout << "This game has 5 ships, a 2-segment rowboat." << endl;
        g.play(p1, p2, false);
        delete p1;
        delete p2;
    }
    else if (line[0] == '2')
    {
        Game g(10, 10);
        addStandardShips(g);
        Player* p1 = createPlayer("mediocre", "Mediocre Midori", g);
        Player* p2 = createPlayer("human", "Shuman the Human", g);
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
    else if (line[0] == '3')
    {
        int nMediocreWins = 0;

        for (int k = 1; k <= 100; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("good", "THE BEAST", g);
            Player* p2 = createPlayer("mediocre", "Mediocre Mimi", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nMediocreWins++;
            delete p1;
            delete p2;
        }
        cout << "The good player won " << nMediocreWins << " out of "
             << "100" << " games." << endl;
          // We'd expect a mediocre player to win most of the games against
          // an awful player.  Similarly, a good player should outperform
          // a mediocre player.
    }
    else if (line[0] == '4')
    {
        Game g(10, 10);
                addStandardShips(g);
                Player* p1 = createPlayer("human", "Mediocre Midori", g);
                Player* p2 = createPlayer("awful", "Shuman the Human", g);
                g.play(p1, p2);
                delete p1;
                delete p2;
    }
    else if (line[0] == '5')
    {
        Game g(2, 3);
        g.addShip(2, 'R', "rowboat");
        Player* p1 = createPlayer("awful", "Popeye", g);
        Player* p2 = createPlayer("awful", "Bluto", g);
        cout << "This mini-game has one ship, a 2-segment rowboat." << endl;
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
    else if (line[0] == '6')
    {
        Game g(10,10);
        addStandardShips(g);
        Player* p1 = createPlayer("good", "Lawrence The Beast", g);
        Player* p2 = createPlayer("mediocre", "Bluto the Fluke", g);
        cout << "This game has 5 ships, a 2-segment rowboat." << endl;
        g.play(p1, p2, false);
        delete p1;
        delete p2;
    }
    else
    {
       cout << "That's not one of the choices." << endl;
    }
    cout << "Do you want to continue?" << endl;
    cin >> cont;
    cin.ignore(10000,'\n');
}while (cont == 'y');
}
