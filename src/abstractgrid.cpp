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

#include <cstdlib> // rand()
#include <QMap>
#include <KDebug>
#include "abstractgrid.h"


AbstractCell::AbstractCell(int index) 
{
    m_index = index;
    m_cables = originalCables = None;
    m_isServer = false;
    m_isConnected = false;
    m_isInOriginalPosition = true;
}
    
// only used to change the cables (not to rotate the cell!)
void AbstractCell::setCables(Directions newCables) 
{
    m_cables = originalCables = newCables;
    m_isInOriginalPosition = true;
}

void AbstractCell::setServer(bool isServer) 
{
    m_isServer = isServer;
}

void AbstractCell::setConnected(bool isConnected) 
{
    m_isConnected = isConnected;
}

void AbstractCell::emptyMove()
{
    m_isInOriginalPosition = false;
}

void AbstractCell::turnClockwise() 
{
    Directions newdirs = None;
    if (m_cables & Up) newdirs = Directions(newdirs | Right);
    if (m_cables & Right) newdirs = Directions(newdirs | Down);
    if (m_cables & Down) newdirs = Directions(newdirs | Left);
    if (m_cables & Left) newdirs = Directions(newdirs | Up);
    m_cables = newdirs;
    m_isInOriginalPosition = false;
}

// TODO: remove next two functions if not used
void AbstractCell::turnCounterclockwise() 
{
    Directions newdirs = None;
    if (m_cables & Up) newdirs = Directions(newdirs | Left);
    if (m_cables & Right) newdirs = Directions(newdirs | Up);
    if (m_cables & Down) newdirs = Directions(newdirs | Right);
    if (m_cables & Left) newdirs = Directions(newdirs | Down);
    m_cables = newdirs;
    m_isInOriginalPosition = false;
}

void AbstractCell::invert() 
{
    turnClockwise();
    turnClockwise();
}

void AbstractCell::reset() 
{
    m_isInOriginalPosition = true;
    m_cables = originalCables;
}


AbstractGrid::AbstractGrid(uint width, uint height, Wrapping wrapping)
{
    m_width = width;
    m_height = height;
    m_isWrapped = wrapping;
    
    // TODO: I'm quite sure the following isn't needed
    // delete old cells
    while (!m_cells.isEmpty()){
        delete m_cells.takeFirst();
    }
    
    // and create new cells
    for (uint i = 0; i < width*height; ++i) {
        m_cells.append(new AbstractCell(i));
    }
    
    createGrid();
    // FIXME: add check
    /*while(!isValid()) {
        createGrid();
    }*/
    
    // shuffle all cells
    for (uint i = 0; i < width*height; ++i) {
        int rotation = rand() % 4; // 0..3
        for (int j = 0; j < rotation; ++j) {
            // ratate every cable clockwise
            m_cells[i]->turnClockwise();
        }
    }
}

AbstractGrid::~AbstractGrid()
{
    while (!m_cells.isEmpty())
        delete m_cells.takeFirst();
}



// ============ auxiliary funciontions ===================== //

void AbstractGrid::createGrid()
{
    // add a random server
    server_index = rand() % (m_width*m_height);
    m_cells[server_index]->setServer(true);
    
    // number of cells that aren't free
    uint cellCount = 0;
    // TODO:use a global constant instead of 10 / 8
    const uint MinimumNumCells = m_width*m_height * 8 / 10;
    // retries until the minimum number of cells is big enough
    while (cellCount < MinimumNumCells) {
        QList<uint> list;
        list.append(server_index);
        if (rand() % 2) addRandomCable(list);
        
        // add some random cables...
        // the list empties if there aren't many free cells left
        // (because of addRandomCable() not doing anything)
        while (!list.isEmpty()) {
            if (rand() % 2) {
                addRandomCable(list);
                if (rand() % 2) addRandomCable(list);
                list.erase(list.begin());
            } 
            else {
                list.append(list.first());
                list.erase(list.begin());
            }
        }
        
        // count not empty cells
        cellCount = 0;
        for (uint i = 0; i < m_width*m_height; ++i) {
            if (m_cells[i]->cables() != None) ++cellCount;
        }
    }
}

// adds a random direction and moves on (if possible)
void AbstractGrid::addRandomCable(QList<uint>& list)
{
    int cell = list.first();
    // find all the cells surrounding list.first() 
    // (0 when cells don't exist)
    int ucell = uCell(cell); // up
    int rcell = rCell(cell); // right
    int dcell = dCell(cell); // down
    int lcell = lCell(cell); // left

    QMap<Directions, int> freeCells;

    // of those cells map the ones that are free
    if (ucell != NO_CELL && m_cells[ucell]->cables() == None) {
        freeCells[Up] = ucell;
    }
    if (rcell != NO_CELL && m_cells[rcell]->cables() == None) {
        freeCells[Right] = rcell;
    }
    if (dcell != NO_CELL && m_cells[dcell]->cables() == None) {
        freeCells[Down] = dcell;
    }
    if (lcell != NO_CELL && m_cells[lcell]->cables() == None) {
        freeCells[Left] = lcell;
    }
    
    if (freeCells.isEmpty()) return; // no free cells left

    QMap<Directions, int>::ConstIterator it = freeCells.constBegin();
    // move the iterator to a random direction connecting to a free cell
    for (int i = rand() % freeCells.count(); i > 0; --i) ++it;
    
    // add the cable in the direction of cell
    Directions newCables = Directions(m_cells[cell]->cables() | it.key());
    m_cells[cell]->setCables(newCables);
    // add a cable in the opposite direction, on the free cell
    m_cells[it.value()]->setCables(invertDirection(it.key()));
    // append the cell that was free to the list
    list.append(it.value());
}

int AbstractGrid::uCell(uint cell) const
{
    if (cell >= m_width)
        return cell - m_width;
    else if (m_isWrapped)
        return m_width * (m_height - 1) + cell;
    else return NO_CELL;
}

int AbstractGrid::dCell(uint cell) const
{
    if (cell < m_width * (m_height - 1))
        return cell + m_width;
    else if (m_isWrapped)
        return cell - m_width * (m_height - 1);
    else return NO_CELL;
}

int AbstractGrid::lCell(uint cell) const
{
    if (cell % m_width > 0)
        return cell - 1;
    else if (m_isWrapped)
        return cell - 1 + m_width;
    else return NO_CELL;
}

int AbstractGrid::rCell(uint cell) const
{
    if (cell % m_width < m_width - 1)
        return cell + 1;
    else if (m_isWrapped)
        return cell + 1 - m_width;
    else return NO_CELL;
}

// TODO: something better to invert directions (remove duplication etc...)
Directions AbstractGrid::invertDirection(Directions givenDirection)
{
    QMap<Directions, Directions> invDirs;
    invDirs[Up]    = Down;
    invDirs[Right] = Left;
    invDirs[Down]  = Up;
    invDirs[Left]  = Right;
    
    return invDirs[givenDirection];
}

/*
bool AbstractGrid::isValid()
{
    MoveList movesDone;
    // initialize the moves
    int index = 0;
    foreach (AbstractCell *cell, m_cells) {
        movesDone.append(Move(cell->cables, index));
        ++index;
    }
    // TODO: remove debug messages
    isSolutionCount = 0;
    int sols = solutions(movesDone);
    kDebug() << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << sols << "count:" << isSolutionCount;
    //return solutions(movesDone) == 1;
    return sols == 1;
}

// I use a depth first search (more memory efficient), 
// i'm only interested in knowing if there are solutions
// other than the "right" one
int AbstractGrid::solutions(MoveList movesDone)
{
    MoveList possibleNextMoves;
    foreach (Move move, movesDone) {
        if (move.noMove()) {
            // TODO: remove double moves (if the cable is a straight line or None)
            move.emptyMove();
            possibleNextMoves.append(move);
            move.turnLeft();
            possibleNextMoves.append(move);
            move.turnLeft();
            possibleNextMoves.append(move);
            move.turnLeft();
            possibleNextMoves.append(move);
        }
    }
    
    // all cells have been moved
    if (possibleNextMoves.isEmpty()) {
        return isSolution(movesDone) ? 1 : 0;
    }
    
    int solutionsFound = 0;
    foreach (Move nextMove, possibleNextMoves) {
        movesDone.replace(nextMove.index(), nextMove);
        if (!check(movesDone)) continue;
        solutionsFound += solutions(movesDone);
    }
    return solutionsFound;
}

bool AbstractGrid::check(MoveList moves) 
{
    foreach (Move move, moves) {
        if (move.noMove()) continue;
        
        uint x = move.index() % m_width;
        uint y = move.index() / m_width;
        Directions cables = move.cables();
        
        // check if there are moved cells near the borders that are wrong
        if (!m_isWrapped) {
            if (x == 0          && cables & Left)  return false;
            if (x == m_width-1  && cables & Right) return false;
            if (y == 0          && cables & Up)    return false;
            if (y == m_height-1 && cables & Down)  return false;
        }
        
        // check if there are contiguous moved cells that are wrong
        
        if (cables & Left) {
            int lcell = lCell(move.index());
            if (lcell != NO_CELL && !moves[lcell].noMove()) {
                // also the cell to the left of the current has been moved
                // if it doesn't connect return false
                if (!(moves[lcell].cables() & Right)) return false;
            }
        }
        
        if (cables & Right) {
            int rcell = rCell(move.index());
            if (rcell != NO_CELL && !moves[rcell].noMove()) {
                if (!(moves[rcell].cables() & Left)) return false;
            }
        }
        
        if (cables & Up) {
            int ucell = uCell(move.index());
            if (ucell != NO_CELL && !moves[ucell].noMove()) {
                if (!(moves[ucell].cables() & Down)) return false;
            }
        }
        
        if (cables & Down) {
            int ucell = uCell(move.index());
            if (ucell != NO_CELL && !moves[ucell].noMove()) {
                if (!(moves[ucell].cables() & Up)) return false;
            }
        }
    }
    return true;
}

bool AbstractGrid::isSolution(MoveList moves) 
{
    // TODO debug only
    ++isSolutionCount;
    
    
    
    updateConnections();
    // return false if there is a terminal that isn't connected
    foreach (Move move, moves) {
        Directions dirs = move.cables();
        if ((dirs == Up || dirs == Right || dirs == Down || dirs == Left) 
                && !m_cells->isConnected) {
            return false;
        }
    }
    // all terminals are connected
    return true;
}

void AbstractGrid::updateConnections()
{
    bool newConnections[m_width * m_height];
    for (uint i = 0; i < m_width * m_height; ++i) {
        newConnections[i] = false;
    }
    
    // indexes of the changed cells
    QList<int> changedCells;
    changedCells.append(server_index);
    m_cells[server_index]->isConnected = true;
    
    while (!changedCells.isEmpty())
    {
        int cell_index = list.first();
        int uindex = uCell(cell_index);
        int rindex = rCell(cell_index);
        int dindex = dCell(cell_index);
        int lindex = lCell(cell_index);
        
        // TODO: aren't needed only use indexes
        AbstractCell *cell = m_cells[cell_index];
        AbstractCell *ucell = (uindex != NO_CELL) ? m_cells[uindex] : 0;
        AbstractCell *rcell = (rindex != NO_CELL) ? m_cells[rindex] : 0;
        AbstractCell *dcell = (dindex != NO_CELL) ? m_cells[dindex] : 0;
        AbstractCell *lcell = (lindex != NO_CELL) ? m_cells[lindex] : 0;

        if ((cell->cables & Up) && ucell != 0 && 
                (ucell->cables & Down) && !newConnections[uindex]) {
            newConnections[uindex] = true;
            list.append(ucell);
        }
        if ((cell->cables & Right) && rcell != 0 && 
                (rcell->cables & Left) && !newConnections[rindex]) {
            newConnections[rindex] = true;
            list.append(rcell);
        }
        if ((cell->cables & Down) && dcell != 0 && 
                (dcell->cables & Up) && !newConnections[dindex]) {
            newConnections[dindex] = true;
            list.append(dcell);
        }
        if ((cell->cables & Left) && lcell != 0 && 
                (lcell->cables & Right) && !newConnections[lindex]) {
            newConnections[lindex] = true;
            list.append(lcell);
        }
        list.erase(list.begin());
    }

    for (int i = 0; i < MasterBoardSize * MasterBoardSize; i++){
        m_cells[i]->isConnected = newConnections[i];
    }
}

// ======================== Move ============================ //

Move::Move(Directions cables) 
{
    m_cables = cables;
    m_noMove = true;
    originalCables = cables;
}

void Move::emptyMove()
{
    m_noMove = false;
}

void Move::turnRight() 
{
    Directions newdirs = None;
    if (m_cables & Up) newdirs = Directions(newdirs | Right);
    if (m_cables & Right) newdirs = Directions(newdirs | Down);
    if (m_cables & Down) newdirs = Directions(newdirs | Left);
    if (m_cables & Left) newdirs = Directions(newdirs | Up);
    m_cables = newdirs;
    m_noMove = false;
}

// TODO: remove next two functions if not used
void Move::turnLeft() 
{
    Directions newdirs = None;
    if (m_cables & Up) newdirs = Directions(newdirs | Left);
    if (m_cables & Right) newdirs = Directions(newdirs | Up);
    if (m_cables & Down) newdirs = Directions(newdirs | Right);
    if (m_cables & Left) newdirs = Directions(newdirs | Down);
    m_cables = newdirs;
    noMove = false;
}

void Move::turnTwice() 
{
    turnLeft();
    turnLeft();
}

// unset all rotations
void Move::unset() 
{
    m_noMove = true;
    m_cables = originalCables;
}*/
