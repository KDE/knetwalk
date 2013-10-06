/*
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
    Copyright 2010 Brian Croom <brian.s.croom@gmail.com>
  
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ABSTRACT_GRID
#define ABSTRACT_GRID

#include <QList>
#include "globals.h"

class AbstractCell
{
public:
    explicit AbstractCell(int index);
    
    virtual ~AbstractCell() {}
    
    Directions cables() const {return m_cables;}
    int index() const {return m_index;}
    bool isServer() const {return m_isServer;}
    bool isConnected() const {return m_isConnected;}
    bool hasBeenMoved() const {return m_hasBeenMoved;}
    bool isTerminal() const;
    
    // should not be used to rotate the cell
    void setCables(Directions newCables);    
    void setServer(bool isServer);    
    virtual void setConnected(bool isConnected);
    
    // sets the cell as if newly created
    virtual void makeEmpty();
    
    void emptyMove(); // sets hasBeenMoved to true
    void rotateClockwise();
    void rotateCounterclockwise();
    void invert(); // rotates the cables by 180 degrees
    
    // reset to the original position
    void reset();
    
    // used for debugging only
    char *toString();
    
private:
    int m_index;
    Directions originalCables;
    Directions m_cables;
    bool m_isServer;
    bool m_isConnected;
    bool m_hasBeenMoved;
};

class Move
{
public:
    enum MoveDirection {None, Left, Right, Inverted};
    Move() {}
    
    Move(int index, MoveDirection move) {
        m_index = index;
        m_move = move;
    }
    
    int index() const {return m_index;}
    MoveDirection move() const {return m_move;}

private:
    int m_index;
    MoveDirection m_move;
};

typedef QList<Move> MoveList;


class AbstractGrid
{
public:
    // creates a grid made of AbstractCells, which will be a valid game
    // this is the main purpose of the class
    AbstractGrid();
    virtual ~AbstractGrid();

    int width() const {return m_width;}
    int height() const {return m_height;}
    int cellCount() const {return m_cells.size();} // TODO: use in the cpp file
    int minimumMoves() const {return m_minimumMoves;}

    // ownership remains to the AbstractGrid
    AbstractCell *cellAt(int index) const {return m_cells[index];}
    void initializeGrid(uint width, uint height, Wrapping w=NotWrapped);
    // updates the connections of the cells
    // returns the indexes of the changed cells
    QList<int> updateConnections();
    // returns true if all terminals are connected to the server
    bool allTerminalsConnected();
    
private:
    // used for debugging only
    void print(); // outputs the grid

    // used when an index of a cell is required
    static const int NO_CELL = -1;

    QList<AbstractCell *> m_cells;
    uint m_width;
    uint m_height;
    bool m_isWrapped;
    
    int server_index;
    int m_minimumMoves;
    
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
    
    // return the number of solutions given a few moves already done
    int solutionCount();
    
    // returns true if you can connect all terminal without usign all cables
    // (doesn't really work as I wanted, doesn't detect most cases)
    bool hasUnneededCables();
    
    // return false if some of the moves done are clearly wrong
    bool movesDoneArePossible();
    // the minimum number of moves required to solve the game

    virtual AbstractCell *newCell(int index) {return new AbstractCell(index);}

    bool isPossibleSolution();
};

#endif // ABSTRACT_GRID
