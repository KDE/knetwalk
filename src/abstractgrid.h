/*
    Copyright 2007-2008 Fela Winkelmolen <fela.kde@gmail.com> 
  
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

struct AbstractCell
{
public:
    AbstractCell(int index);
    
    Directions cables() const {return m_cables;}
    int index() const {return m_index;}
    bool isServer() const {return m_isServer;}
    bool isConnected() const {return m_isConnected;}
    bool hasBeenMoved() const {return !m_isInOriginalPosition;}
    bool isTerminal() const;
    
    // only used to change the cables (not to rotate the cell!)
    void setCables(Directions newCables);    
    void setServer(bool isServer=true);    
    void setConnected(bool isConnected=true);
    
    void emptyMove(); // sets isInOriginalPosition to false
    void turnClockwise();
    void turnCounterclockwise();
    void invert(); // turns the cables by 180 degrees
    
    void reset();
    
    char *toString();
    
protected: // TODO: maybe could be private..
    int m_index;
    Directions originalCables;
    Directions m_cables;
    bool m_isServer;
    bool m_isConnected;
    bool m_isInOriginalPosition; // TODO: change to !m_hasBeenMoved
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
    
    int index() {return m_index;}
    MoveDirection move() {return m_move;}

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
    AbstractGrid(uint width, uint height, Wrapping w=NotWrapped); 
    ~AbstractGrid();
    
    // ownership remains to the AbstractGrid
    QList<AbstractCell *> cells() const {return m_cells;}
    
    void print(); // outputs the grid
    
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
    
    
    bool isValid();
    
    // return the number of solutions given a few moves already done
    int solutions();
    
    // return false if the moves are clearly impossible
    bool check();
    
    // checks if moves is a solution
    bool isSolution();
    
    // updates the connections of the squares
    void updateConnections();
};

#endif // ABSTRACT_GRID
