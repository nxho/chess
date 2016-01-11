#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cctype>
#include <cmath>
#include <map>
using namespace std;

class Coord {
    int l;
    int n;
public:
    Coord():l(0), n(0) { }
    Coord(int a, int b):l(a), n(b) { }
    int getL() {
        return l;
    }
    int getN() {
        return n;
    }
    void setLN(int a, int b) {
        l = a;
        n = b;
    }
};

typedef multimap<char, Coord> PieceMap;

class Board {
    char grid[8][8]; // grid[n][l]
    char turn;
    PieceMap locationMap;
public:
    Board() {
        char pieces[9] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r', 'p'};

        for(int i = 0; i < 8; i++) {
            grid[7][i] = pieces[i];
            grid[6][i] = pieces[8];
            grid[5][i] = ' ';
            grid[4][i] = ' ';
            grid[3][i] = ' ';
            grid[2][i] = ' ';
            grid[1][i] = toupper(pieces[8]);
            grid[0][i] = toupper(pieces[i]);
            locationMap.insert(pair<char, Coord>(pieces[i], Coord(i, 7)));
            locationMap.insert(pair<char, Coord>(pieces[8], Coord(i, 6)));
            locationMap.insert(pair<char, Coord>(toupper(pieces[8]), Coord(i, 1)));
            locationMap.insert(pair<char, Coord>(toupper(pieces[i]), Coord(i, 0)));
        }

        turn = 'w';
    }

    char getTurn() {
    	return turn;
    }

    void toggleTurn() {
    	if(turn == 'w')
    		turn = 'b';
    	else
    		turn = 'w';
    }

    double calcDistance(Coord c1, Coord c2) {
        return sqrt(pow((double)(c1.getL() - c2.getL()), 2.0) + pow((double)(c1.getN() - c2.getN()), 2.0));
    }

    bool checkInterceptPiece(Coord src, Coord dst) {
        int dist_l = dst.getL() - src.getL(),
            dist_n = dst.getN() - src.getN(),
            offset_l = 1, offset_n = 1;

        if(dist_l < 0)
            offset_l = -1;
        if(dist_l == 0)
            offset_l = 0;
        if(dist_n < 0)
            offset_n = -1;
        if(dist_n == 0)
            offset_n = 0;
        //cout << "dist: "<< dist_l << ' ' << dist_n << endl;
        //cout << "offset: " << offset_l << ' ' << offset_n << endl;

        dist_l--;
        dist_n--;
        for(int l = src.getL() + offset_l, n = src.getN() + offset_n;
                dist_l > 0 || dist_n > 0;
                dist_l--, dist_n--, l+=offset_l, n+=offset_n) {
            if(grid[n][l] != ' ')
                return false;
        }
        return true;
    }

    bool checkRook(Coord src, Coord dst) {
        return (src.getL() == dst.getL() || src.getN() == dst.getN());
    }

    bool checkBishop(Coord src, Coord dst) {
        return (abs(src.getL() - dst.getL()) == abs(src.getN() - dst.getN()));
    }

    bool checkPawn(Coord src, Coord dst) {
        char pieceDst = grid[dst.getN()][dst.getL()];
        int i = 1,
            j = 2;

        if(getTurn() == 'b') {
            i = -1;
            j = -2;
        }
        //cout << "src: " << '(' << src.getL() << ", " << src.getN() << ")\n";
        //cout << "dst: " << '(' << dst.getL() << ", " << dst.getN() << ")\n";

        return (((src.getL() == dst.getL())
                && (pieceDst == ' ')
                && ((dst.getN() - src.getN() == i) // first check if it is a regular pawn move (forward 1 square)
                    // pawn's first move?
                    || ((dst.getN() - src.getN() == j)
                        // check if it is on starting row, depending on its color
                        && ((getTurn() == 'b' && src.getN() == 6)
                            || (getTurn() == 'w' && src.getN() == 1))
                        && checkInterceptPiece(src, dst))))
                // check if is capture
                || (dst.getN() - src.getN() == i
                    && calcDistance(src,dst) == sqrt(2.0)
                    && pieceDst != ' '));

        // still need to account for en passant
    }

    bool isValidMove(Coord src, Coord dst) {
        double dist = calcDistance(src, dst);
        char piece = grid[src.getN()][src.getL()];

        switch(tolower(piece)) {
            case 'r':
                return checkRook(src, dst) && checkInterceptPiece(src, dst);
            case 'n':
                //cout << "src: " << '(' << src.getL() << ", " << src.getN() << ")\n";
                //cout << "dst: " << '(' << dst.getL() << ", " << dst.getN() << ")\n";
                //cout << dist << ' ' << sqrt(5.0) << endl;
                return (dist == sqrt(5.0));
            case 'b':
                return checkBishop(src, dst) && checkInterceptPiece(src, dst);
            case 'q':
                return ((checkRook(src, dst) || checkBishop(src, dst)) && checkInterceptPiece(src, dst));
            case 'k':
                return (dist == 1.0 || dist == sqrt(2.0));
            case 'p':
                return checkPawn(src, dst);
            default:
                return false;
        }
    }

    void move(Coord& src, Coord dst) {
        grid[dst.getN()][dst.getL()] = grid[src.getN()][src.getL()];
        grid[src.getN()][src.getL()] = ' ';
    }

    bool parseInput(char* input) {
        Coord dst;
        int i = 1, j = 2;
        char piece = toupper(input[0]);

        if(input[0] >= 'a' && input[0] <= 'h') {
            piece = 'P';
            i = 0;
            j = 1;
        }

        dst.setLN((int)(input[i] - 'a'), (int)(input[j] - '0') - 1);

        //cout << piece << ' ' << dst.getL() << ' ' << dst.getN();

        // find the source piece
        if(getTurn() == 'b')
            piece = tolower(piece);
        pair<PieceMap::iterator, PieceMap::iterator> range = locationMap.equal_range(piece);
        PieceMap::iterator itr = range.first;
        for(; itr != range.second; itr++) {
            //cout << itr->second.getL() << ' ' << itr->second.getN() << endl;
            if(isValidMove(itr->second, dst))
                break;
        }
        if(itr != range.second) {
            move(itr->second, dst);
            itr->second = dst;
            return true;
        }
        cout << "invalid move\n";
        return false;
    }

    void printBoard() {
        cout << setw(8*6+1) << setfill('-') << "-" << endl;
        cout << setw((8*6+6)/2) << setfill(' ') << "CHESS" << endl;
        cout << setw(8*6+1) << setfill('-') << "-" << endl;
        for(int i = 7; i >= 0; i--) {
            for(int j = 0; j < 3; j++) {
                for(int k = 0; k < 8; k++) {
                    if((i + k) % 2 == 0) {
                        switch(j) {
                            case 0:
                                cout << "|.    ";
                                break;
                            case 1:
                                cout << "|  " << grid[i][k] << "  ";
                                break;
                            case 2:
                                cout << "|_____";
                                break;
                        }
                    }
                    else {
                        switch(j) {
                            case 0:
                                cout << left << setw(6) << setfill(' ') << '|';
                                break;
                            case 1:
                                cout << "|  " << grid[i][k] << "  ";
                                break;
                            case 2:
                                cout << left << setw(6) << setfill('_') << '|';
                                break;
                        }
                    }
                }
                cout << "|";
                if(j == 1)
                    cout << " " << i + 1;
                cout << endl;
            }
        }
        for(int i = 0; i < 8; i++)
            cout << "   " << char(i + 'a') << "  ";
        cout << endl;
    }

    void printSquare(int l, int n, int i) {

    }
};

int main() {
    char input[8];
    Board board;

    board.printBoard();
 
    while(1) {
    	if(board.getTurn() == 'w')
    		cout << "\nPlayer 1 (white). Please enter a move: ";
    	else
    		cout << "\nPlayer 2 (black). Please enter a move: ";
        cin.getline(input, 8);
        if(!board.parseInput(input))
            continue;
        board.toggleTurn();
        board.printBoard();
    }

    cout << endl;
}
