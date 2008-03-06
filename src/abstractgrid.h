#ifndef ABSTRACT_GRID
#define ABSTRACT_GRID

#include <QList>
#include "globals.h"

struct AbstractCell
{
public:
    AbstractCell(int index);
    
    Directions cables() const {return m_cables;}
    int index() const {return m_index;}
    bool isServer() const {return m_isServer;}
    bool isConnected() const {return m_isConnected;}
    bool isInOriginalPosition() const {return m_isInOriginalPosition;}
    
    // only used to change the cables (not to rotate the cell!)
    void setCables(Directions newCables);    
    void setServer(bool isServer=true);    
    void setConnected(bool isConnected=true);
    
    void emptyMove(); // sets isInOriginalPosition to false
    void turnClockwise();
    void turnCounterclockwise();
    void invert(); // turns the cables by 180 degrees
    
    void reset();
    
protected: // TODO: maybe could be private..
    int m_index;
    Directions originalCables;
    Directions m_cables;
    bool m_isServer;
    bool m_isConnected;
    bool m_isInOriginalPosition;
};

/*
class Move
{
public:
    Move(Directions cables, int index);
    
    Directions cables() const {return m_cables;}
    bool noMove() const {return m_noMove;}
    int index() const {return m_index;}
    
    void emptyMove(); // set noMove to false
    void turnRight();
    void turnLeft();
    void turnTwice(); // turns the cables by 180 degrees
    
    // unset all rotations
    void unset();
    
private:
    int m_index;
    Directions originalCables;
    Directions m_cables;
    bool m_noMove;
};

typedef QList<Move> MoveList;
*/

class AbstractGrid
{
public:
    // creates a grid made of AbstractCells, which will be a valid game
    // this is the main purpose of the class
    AbstractGrid(uint width, uint height, Wrapping w=NotWrapped); 
    ~AbstractGrid();
    
    // ownership remains to the AbstractGrid
    QList<AbstractCell *> cells() const {return m_cells;}
    //int width() {return m_width;}
    //int height() {return m_height;}
    //bool isWrapped() {return m_isWrapped;}
    
private:
    // TODO: debug only
    int isSolutionCount;

    static const int NO_CELL = -1;

    QList<AbstractCell *> m_cells;
    uint m_width;
    uint m_height;
    bool m_isWrapped;
    
    int server_index;
    
    //======== auxiliary functions ========//
    void createGrid(); // only used for modularization
    
    // adds a random direction (cable) and moves on (if possible)
    // used when creating the grid
    void addRandomCable(QList<uint>& list);
    // find the index to the left/right/up/down w.r.t the index given
    int uCell(uint cell) const;
    int dCell(uint cell) const;
    int lCell(uint cell) const;
    int rCell(uint cell) const;
    
    // return the opposite direction of the one given
    // !!! a single direction is expected as parameter (not bitwise ORed!!) !!!
    Directions invertDirection(Directions givenDirection);
    
    /*
    bool isValid();
    
    // return the number of solutions given a few moves already done
    int solutions(MoveList moves);
    
    // return false if the moves are clearly impossible
    bool check(MoveList moves);
    
    // checks if moves is a solution
    bool isSolution(MoveList moves);
    
    // updates the connections of the squares
    void updateConnections();*/
};

#endif // ABSTRACT_GRID
