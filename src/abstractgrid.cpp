/*
    Copyright 2004-2005 Andi Peredri <andi@ukr.net>   
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
#include <unistd.h> // sleep()
#include <QMap>
#include <QString>
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

char *AbstractCell::toString() {
    char *str = new char[4];
    str[0] = (m_cables & Left) ? '-' : ' ';
    str[2] = (m_cables & Right) ? '-' : ' ';
    if ((m_cables & Up) && (m_cables & Down)) {
        str[1] = '|';
    } else if (m_cables & Up) {
        str[1] = '\'';
    } else if (m_cables & Down) {
        str[1] = ',';
    } else if ((m_cables & Left) && (m_cables & Right)){
        str[1] = '-';
    } else {
        str[1] = ' ';
    }
    str[3] = '\0';
    
    return str;
}

bool AbstractCell::isTerminal() const 
{
    return (m_cables == Up || m_cables == Right 
            || m_cables == Down || m_cables == Left);
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
    
    createGrid();
    
    while(hasUnneededCables() || solutionCount() != 1) {
        // the grid is invalid: create a new one
        createGrid();
    }
    
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

void AbstractGrid::print() {
    system("clear");
    QString str1;
    QString str2;
    int index = 0;
    for (uint r = 0; r < m_height; ++r) {
        for (uint c = 0; c < m_width; ++c) {
            str1 += m_cells[index]->toString();
            str1 += "  ";
            if (m_cells[index]->hasBeenMoved()) {
                str2 += "M ";
            } else {
                str2 += "  ";
            }
            ++index;
        }
        kDebug() << str1 << "     " << str2;
        kDebug() << " ";
        str1 = str2 = "";
    }
}

// ============ auxiliary funciontions ===================== //

void AbstractGrid::createGrid()
{
    while (!m_cells.isEmpty()){
        delete m_cells.takeFirst();
    }
    
    // and create new cells
    for (uint i = 0; i < m_width*m_height; ++i) {
        m_cells.append(new AbstractCell(i));
    }

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
    if (cell >= m_width) {
        return cell - m_width;
    } else if (m_isWrapped) {
        return m_width * (m_height - 1) + cell;
    } else {
        return NO_CELL;
    }
}

int AbstractGrid::dCell(uint cell) const
{
    if (cell < m_width * (m_height - 1)) {
        return cell + m_width;
    } else if (m_isWrapped) {
        return cell - m_width * (m_height - 1);
    } else {
        return NO_CELL;
    }
}

int AbstractGrid::lCell(uint cell) const
{
    if (cell % m_width > 0) {
        return cell - 1;
    } else if (m_isWrapped) {
        return cell - 1 + m_width;
    } else {
        return NO_CELL;
    }
}

int AbstractGrid::rCell(uint cell) const
{
    if (cell % m_width < m_width - 1) {
        return cell + 1;
    } else if (m_isWrapped) {
        return cell + 1 - m_width;
    } else {
        return NO_CELL;
    }
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


int AbstractGrid::solutionCount()
{
    MoveList possibleNextMoves;
    // TODO: put following in external function
    foreach (AbstractCell *cell, m_cells) {
        if (!cell->hasBeenMoved()) {
            Directions dirs = cell->cables();
            Move move;
            if (dirs == None) {
                // no cables
                move = Move(cell->index(), Move::None);
                possibleNextMoves.append(move);
            } else if (dirs == (Up | Down) || dirs == (Left | Right)) {
                // cables forming a line
                move = Move(cell->index(), Move::None);
                possibleNextMoves.append(move);
                
                move = Move(cell->index(), Move::Left);
                possibleNextMoves.append(move);
            } else {
                // other kind of cables
                move = Move(cell->index(), Move::None);
                possibleNextMoves.append(move);
                
                move = Move(cell->index(), Move::Left);
                possibleNextMoves.append(move);
                
                move = Move(cell->index(), Move::Right);
                possibleNextMoves.append(move);
                
                move = Move(cell->index(), Move::Inverted);
                possibleNextMoves.append(move);
            }
            break;
        }
    }
    
    // all cells have been moved
    if (possibleNextMoves.isEmpty()) {
        return isSolution() ? 1 : 0;
    }
    // else
    
    int solutionsFound = 0;
    foreach (Move nextMove, possibleNextMoves) {
        int index = nextMove.index();
        
        switch (nextMove.move()) {
        case Move::None:
            m_cells[index]->emptyMove();
            break;
        case Move::Right:
            m_cells[index]->turnClockwise();
            break;
        case Move::Left:
            m_cells[index]->turnCounterclockwise();
            break;
        case Move::Inverted:
            m_cells[index]->invert();
            break;
        }
        
        if (movesDoneArePossible()) {
            solutionsFound += solutionCount(); // recursive call
        }
        
        m_cells[index]->reset(); // undo move
    }
    return solutionsFound;
}

bool AbstractGrid::movesDoneArePossible() 
{
    foreach (AbstractCell *cell, m_cells) {
        if (!cell->hasBeenMoved()) continue;
        
        uint x = cell->index() % m_width;
        uint y = cell->index() / m_width;
        Directions cables = cell->cables();
        
        // check if there are moved cells near the borders that are wrong
        if (!m_isWrapped) {
            if (x == 0          && cables & Left)  return false;
            if (x == m_width-1  && cables & Right) return false;
            if (y == 0          && cables & Up)    return false;
            if (y == m_height-1 && cables & Down)  return false;
        }
        
        // check if there are contiguous moved cells that are wrong
        
        if (cables & Left) {
            int lcell = lCell(cell->index());
            if (lcell != NO_CELL && m_cells[lcell]->hasBeenMoved()) {
                // also the cell to the left of the current has been moved
                
                // if it doesn't connect return false
                if (!(m_cells[lcell]->cables() & Right)) return false;
            }
        }
        if (cables & Right) {
            int rcell = rCell(cell->index());
            if (rcell != NO_CELL && m_cells[rcell]->hasBeenMoved()) {
                if (!(m_cells[rcell]->cables() & Left)) return false;
            }
        }
        if (cables & Up) {
            int ucell = uCell(cell->index());
            if (ucell != NO_CELL && m_cells[ucell]->hasBeenMoved()) {
                if (!(m_cells[ucell]->cables() & Down)) return false;
            }
        }
        if (cables & Down) {
            int dcell = dCell(cell->index());
            if (dcell != NO_CELL && m_cells[dcell]->hasBeenMoved()) {
                if (!(m_cells[dcell]->cables() & Up)) return false;
            }
        }
    }
    
    // nothing was wrong
    return true;
}

bool AbstractGrid::hasUnneededCables()
{
    foreach (AbstractCell *cell, m_cells) {
        if (cell->isTerminal() || cell->isServer()) continue;
        
        Directions oldCables = cell->cables();
        cell->setCables(None);
        
        bool solution = isSolution();
        cell->setCables(oldCables);
        
        if (solution) {
            // it has a solution also when the cables of cell are removed
            return true;
        }
    }
    
    return false;
}

bool AbstractGrid::isSolution() 
{
    updateConnections();
    // return false if there is a terminal that isn't connected
    foreach (AbstractCell *cell, m_cells) {
        if (cell->isTerminal() && !cell->isConnected()) {
            return false;
        }
    }
    // all terminals are connected
    return true;
}

void AbstractGrid::updateConnections()
{
    // TODO: add int AbstractGrid::cellsCount()
    QVector<bool> newConnections(m_width * m_height);
    for (uint i = 0; i < m_width * m_height; ++i) {
        newConnections[i] = false;
    }
    
    // indexes of the changed cells
    QList<int> changedCells;
    changedCells.append(server_index);
    m_cells[server_index]->setConnected(true);
    
    while (!changedCells.isEmpty())
    {
        int cell_index = changedCells.first();
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

        if ((cell->cables() & Up) && ucell != 0 && 
                (ucell->cables() & Down) && !newConnections[uindex]) {
            newConnections[uindex] = true;
            changedCells.append(ucell->index());
        }
        if ((cell->cables() & Right) && rcell != 0 && 
                (rcell->cables() & Left) && !newConnections[rindex]) {
            newConnections[rindex] = true;
            changedCells.append(rcell->index());
        }
        if ((cell->cables() & Down) && dcell != 0 && 
                (dcell->cables() & Up) && !newConnections[dindex]) {
            newConnections[dindex] = true;
            changedCells.append(dcell->index());
        }
        if ((cell->cables() & Left) && lcell != 0 && 
                (lcell->cables() & Right) && !newConnections[lindex]) {
            newConnections[lindex] = true;
            changedCells.append(lcell->index());
        }
        changedCells.erase(changedCells.begin());
    }

    for (uint i = 0; i < m_width * m_height; i++){
        m_cells[i]->setConnected(newConnections[i]);
    }
}

