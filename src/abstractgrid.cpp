#include <cstdlib> // rand()
#include <QMap>
#include <KDebug>
#include "abstractgrid.h"

AbstractGrid::AbstractGrid(uint width, uint height, Wrapping wrapping)
{
    m_width = width;
    m_height = height;
    m_isWrapped = wrapping;
    
    // delete old cells
    while (!m_cells.isEmpty()){
        delete m_cells.takeFirst();
    }
    
    // and create new cells
    for (uint i = 0; i < width*height; ++i) {
        m_cells.append(new AbstractCell);
    }
    
    createGrid();
    
    // shuffle all cells
    for (uint i = 0; i < width*height; ++i) {
        int rotation = rand() % 4; // 0..3
        for (int j = 0; j < rotation; ++j) {
            // ratate every cable clockwise
            // TODO: maybe find a more elegant way (duplicated!)
            Directions newCables = None;
            if (m_cells[i]->cables & Up)    
                newCables = Directions(newCables | Right);
            if (m_cells[i]->cables & Right) 
                newCables = Directions(newCables | Down);
            if (m_cells[i]->cables & Down)  
                newCables = Directions(newCables | Left);
            if (m_cells[i]->cables & Left)  
                newCables = Directions(newCables | Up);
            m_cells[i]->cables = newCables;
        }
    }
    
    // FIXME: put somewhere else... (not in Abstract*) 
    //updateConnections();
    //KGameDifficulty::setRunning(false);// setRunning(true) on the first click
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
    int server = rand() % (m_width*m_height);
    m_cells[server]->isServer = true;
    
    // number of cells that aren't free
    uint cellCount = 0;
    // TODO:use a constant instead of 10 / 8
    const uint MinimumNumCells = m_width*m_height * 8 / 10;
    // retries until the minimum number of cells is big enough
    while (cellCount < MinimumNumCells) {
        QList<uint> list;
        list.append(server);
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
            if (m_cells[i]->cables != None) ++cellCount;
        }
    }
}

// adds a random direction and moves on (if possible)
void AbstractGrid::addRandomCable(QList<uint>& list)
{
    const int NO_CELL = -1;
    int cell = list.first();
    // find all the cells surrounding list.first() 
    // (0 when cells don't exist)
    int ucell = uCell(cell); // up
    int rcell = rCell(cell); // right
    int dcell = dCell(cell); // down
    int lcell = lCell(cell); // left

    QMap<Directions, int> freeCells;

    // of those cells map the ones that are free
    if (ucell != NO_CELL && m_cells[ucell]->cables == None) {
        freeCells[Up] = ucell;
    }
    if (rcell != NO_CELL && m_cells[rcell]->cables == None) {
        freeCells[Right] = rcell;
    }
    if (dcell != NO_CELL && m_cells[dcell]->cables == None) {
        freeCells[Down] = dcell;
    }
    if (lcell != NO_CELL && m_cells[lcell]->cables == None) {
        freeCells[Left] = lcell;
    }
    
    if (freeCells.isEmpty()) return; // no free cells left

    QMap<Directions, int>::ConstIterator it = freeCells.constBegin();
    // move the iterator to a random direction connecting to a free cell
    for (int i = rand() % freeCells.count(); i > 0; --i) ++it;
    
    // add the cable in the direction of cell
    m_cells[cell]->cables = Directions(m_cells[cell]->cables | it.key());
    // add a cable in the opposite direction, on the free cell
    m_cells[it.value()]->cables = invertDirection(it.key());
    // append the cell that was free to the list
    list.append(it.value());
}

int AbstractGrid::uCell(uint cell) const
{
    if (cell >= m_width)
        return cell - m_width;
    else if (m_isWrapped)
        return m_width * (m_height - 1) + cell;
    else return -1;
}

int AbstractGrid::dCell(uint cell) const
{
    if (cell < m_width * (m_height - 1))
        return cell + m_width;
    else if (m_isWrapped)
        return cell - m_width * (m_height - 1);
    else return -1;
}

int AbstractGrid::lCell(uint cell) const
{
    if (cell % m_width > 0)
        return cell - 1;
    else if (m_isWrapped)
        return cell - 1 + m_width;
    else return -1;
}

int AbstractGrid::rCell(uint cell) const
{
    if (cell % m_width < m_width - 1)
        return cell + 1;
    else if (m_isWrapped)
        return cell + 1 - m_width;
    else return -1;
}

Directions AbstractGrid::invertDirection(Directions givenDirection)
{
    QMap<Directions, Directions> invDirs;
    invDirs[Up]    = Down;
    invDirs[Right] = Left;
    invDirs[Down]  = Up;
    invDirs[Left]  = Right;
    
    return invDirs[givenDirection];
}

/*bool AbstractGrids::isValid()
{
    moves <- empty;
    hasWrongSolutions(moves);
}

// I use a depth first search (more memory efficient), 
// i'm only interested in knowing if there are solutions
// other than the "right" one
list AbstractGrid::hasWrongSolutions(moves)
{
    list nextStep = ...; // all possible steps
    foreach (ns in nextStep) {
        if (!check(list + ns)) continue;
        if hasWrongSolutions (list + ns, all_cells) return true;
    }
    return false; // no wrong solution
}

// checks if list are valid cells
bool AbstractGrid::check(moves) 
{
    
}*/

