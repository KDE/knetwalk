#ifndef ABSTRACT_GRID
#define ABSTRACT_GRID

#include <QList>
#include "globals.h"

struct AbstractCell
{
public:
    AbstractCell() { // default constructor
        cables = None;
        isServer = false;
    }
    Directions cables;
    bool isServer;
};

class AbstractGrid
{
public:
    // creates a grid made of AbstractCells, which will be a valid game
    // this is the main purpose of the class
    AbstractGrid(uint width, uint height, Wrapping w=NotWrapped); 
    ~AbstractGrid();
    
    // ownership remains to the AbstractGrid
    QList<AbstractCell *> cells() {return m_cells;}
    //int width() {return m_width;}
    //int height() {return m_height;}
    //bool isWrapped() {return m_isWrapped;}
    
private:
    QList<AbstractCell *> m_cells;
    uint m_width;
    uint m_height;
    bool m_isWrapped;
    
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
};

/*
class Move
{
};

typedef QList<Move *> MoveList;

// or maybe just use an array/list of int (index is the cell, value
// is the rotation)
*/

#endif // ABSTRACT_GRID
