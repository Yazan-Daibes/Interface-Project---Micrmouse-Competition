// #include "ArduinoSTL.h"
#include <string>
#include "Cell.h"
#include <stack>
#include <iostream>
#include <cstdlib>
#include "Coordinates.h"

// #include "StackArray.h"
#include "API.h"


int mazeSize = 8; // could be changed to 16X16 maze
int centerX = mazeSize / 2;
int centerY = mazeSize / 2;

// Environment Variables
char orientation = 'N'; // current orientation

int x = 0, y = 0; // current coordinates

//---------------------function signatures--------------------

bool isGoal(int x, int y);
bool isSafe(int X, int Y);
void updateposition(char currentMove);
void exploreCell(Cell cell_arr[][8],int x, int y);
void initializeMaze(Cell cell_arr[][8]);

void findGoal(Cell cell_arr[][8]);
Cell &getFrontCell(Cell cell_arr[][8]);
Cell &getLeftCell(Cell cell_arr[][8]);
Cell &getRightCell(Cell cell_arr[][8]);
void getMinDistanceDirection(Cell cell_arr[][8], int &minDistance, char &minDirection);
void floodOpenNeighbours(Cell cell_arr[][8]);
void moveInDirection(Cell cell_arr[][8], char direction);

//---------------------main()--------------
int main(int argc, char *argv[])
{

    cout << "000000" << "\n";
   // vector<vector<Cell> > map(mazeSize, vector<Cell>(mazeSize));
    Cell cell_arr [8][8];


   std::cout << "111111" << "\n";

    initializeMaze(cell_arr);

    cout << "22222222" << "\n";


  findGoal(cell_arr);

  return 0;
}
//---------------------- function defintions-------------------------------
bool isGoal(int x, int y)
{

    return ((x == 3 || x == 4) && (y == 3 || y == 4));
}



void updateposition(char currentMove)
{
    // update the orientation and coordinates
    switch (orientation)
    {
    case 'N':
        if (currentMove == 'l')
            orientation = 'W';
        else if (currentMove == 'r')
            orientation = 'E';

        else if (currentMove == 'f')
            ++y;
        break;
    case 'S':
        if (currentMove == 'l')
            orientation = 'E';
        else if (currentMove == 'r')
            orientation = 'W';
        else if (currentMove == 'f')
            --y;
        break;
    case 'E':
        if (currentMove == 'l')
            orientation = 'N';
        else if (currentMove == 'r')
            orientation = 'S';
        else if (currentMove == 'f')
            ++x;
        break;
    case 'W':
        if (currentMove == 'l')
            orientation = 'S';
        else if (currentMove == 'r')
            orientation = 'N';
        else if (currentMove == 'f')
            --x;
        break;
    default:
        x = 0, y = 0;
    }
}

void exploreCell(Cell cell_arr[][8],int x, int y)
{
    // map[x][y].setVisited(); // first visit is straight, then right, then left, then back
    if (cell_arr[x][y].getHasBeenExplored())
        return;

    cell_arr[x][y].sethasBeenExplored(true);

    switch (orientation)
    {
    case 'N':

        cell_arr[x][y].setNorthWall(API::wallFront());

        cell_arr[x][y].setEastWall(API::wallRight());
        cell_arr[x][y].setWestWall(API::wallLeft());

        break;
    case 'S':

        cell_arr[x][y].setSouthWall(API::wallFront());
        cell_arr[x][y].setEastWall(API::wallLeft());
        cell_arr[x][y].setWestWall(API::wallRight());
        break;
    case 'W':

        cell_arr[x][y].setWestWall(API::wallFront());
        cell_arr[x][y].setSouthWall(API::wallLeft());
        cell_arr[x][y].setNorthWall(API::wallRight());
        break;
    case 'E':

        cell_arr[x][y].setEastWall(API::wallFront());
        cell_arr[x][y].setNorthWall(API::wallLeft());
        cell_arr[x][y].setSouthWall(API::wallRight());
        break;
    default:
        int a = 0;
    }
}

void initializeMaze(Cell cell_arr[][8])
{
    // for (int i = 0; i < mazeSize; i++)
    // {
    //     map[i].reserve(mazeSize);
    // }

    for (int i = 0; i < mazeSize; i++)
    {
        for (int j = 0; j < mazeSize; j++)
        {
            // cell_arr[i][j].setCordinates(i,j);
            cell_arr[i][j].setcellAddress(Coordinates{i, j});

            int minX = std::min(std::abs(i - centerX), std::abs(i - (centerX - 1)));
            int minY = std::min(std::abs(j - centerY), std::abs(j - (centerY - 1)));
            int floodFillCost = minX + minY;
            cell_arr[i][j].setFloodFillCost(floodFillCost);
           // std::cout << "floodFillCost = " <<  floodFillCost <<"\n";

        }
    }

    cell_arr[0][0].setSouthWall(true); // leads to errors
}



void findGoal(Cell cell_arr[][8] ) // takes coordinates of current cell
{
    bool destinationFound = false;
    int minDistance;
    char minDirection;
    // alias for conveneience
    while (!destinationFound)
    {

        exploreCell(cell_arr, x, y); // explore current cell
        minDistance = mazeSize * 2;
        // if goal fount-> exit while loop
        if (isGoal(x, y))
            destinationFound = true;

        getMinDistanceDirection(cell_arr, minDistance, minDirection);
        // check if reflooding is required

        if (cell_arr[x][y].getFloodFillCost() != 1 + minDistance)
        {
            // reflood
            floodOpenNeighbours(cell_arr);

            // after reflooding get new min distance neighbours
            getMinDistanceDirection(cell_arr, minDistance, minDirection); //get neighbour with lowest distance
        }

        moveInDirection(cell_arr, minDirection);
    }
}

// f-move front ,b- move back,r-move right, l-moveleft
void moveInDirection(Cell cell_arr[][8], char direction)
{
    // move to the  neighbouring cell with the lowest distance cost
    int prevX = x, prevY = y;
    if (direction == 'f')
    {
       API::moveForward();
        updateposition('f');
    }
    else if (direction == 'l')
    {
      API::turnLeft();
        updateposition('l');
       API::moveForward();
        updateposition('f');
    }

    else if (direction == 'r')
    {

       API::turnRight();
        updateposition('r');
       API::moveForward();
        updateposition('f');
    }
    else if (direction == 'b')
    {
       API::turnRight();
        updateposition('r');
       API::turnRight();
        updateposition('r');
       API::moveForward();
        updateposition('f');
    }

    cell_arr[x][y].setPrevVisitedCell(&cell_arr[prevX][prevY]);
}
// doesnt check if coordinates are within boundaries
Cell &getFrontCell(Cell cell_arr[][8])
{

    switch (orientation)
    {
    case 'N':
        return cell_arr[x][y + 1];
        break;
    case 'S':
        return cell_arr[x][y - 1];
        break;
    case 'E':
        return cell_arr[x + 1][y];
        break;
    case 'W':
        return cell_arr[x - 1][y];
        break;
    default:
        return cell_arr[x][y];
    }
}
Cell &getLeftCell(Cell cell_arr[][8])
{

    switch (orientation)
    {
    case 'N':
        return cell_arr[x - 1][y];
        break;
    case 'S':
        return cell_arr[x + 1][y];
        break;
    case 'E':
        return cell_arr[x][y + 1];
        break;
    case 'W':
        return cell_arr[x][y - 1];
        break;
    default:
        return cell_arr[x][y];
        ;
    }
}
Cell &getRightCell(Cell cell_arr[][8])
{

    switch (orientation)
    {
    case 'N':
        return cell_arr[x + 1][y];
        break;
    case 'S':
        return cell_arr[x - 1][y];
        break;
    case 'E':
        return cell_arr[x][y - 1];
        break;
    case 'W':
        return cell_arr[x][y + 1];
        break;
    default:
        return cell_arr[x][y];
    }
}

//get min distance relative to current position and relative position f = front, b = behind, l=left,r=right
void getMinDistanceDirection(Cell cell_arr[][8], int &minDistance, char &minDirection)
{
    if (!API::wallFront())
    {
        Cell front = getFrontCell(cell_arr);

        if (minDistance > front.getFloodFillCost())
        {
            minDistance = front.getFloodFillCost();
            minDirection = 'f';
        }
    }
    if (!API::wallLeft())
    {
        Cell left = getLeftCell(cell_arr);
        if (minDistance > left.getFloodFillCost())
        {
            minDistance = left.getFloodFillCost();
            minDirection = 'l';
        }
    }
    if (!API::wallRight())
    {
        Cell right = getRightCell(cell_arr);
        if (minDistance > right.getFloodFillCost())
        {
            minDistance = right.getFloodFillCost();
            minDirection = 'r';
        }
    }
    if (cell_arr[x][y].getPrevVisitedCell() != NULL)
    {
        Cell *back = cell_arr[x][y].getPrevVisitedCell();
        if (minDistance > back->getFloodFillCost())
        {
            minDistance = back->getFloodFillCost();
            minDirection = 'b';
        }
    }
}

void floodOpenNeighbours(Cell cell_arr[][8])
{

    stack<Coordinates> floodStack;
    int minDistance = mazeSize * 2;
    char minDirection;
    int cellX, cellY;
    floodStack.push(Coordinates({x, y}));
    while (!floodStack.empty())
    {
        cellX = floodStack.top().x;
        cellY = floodStack.top().y;

        floodStack.pop();
        if (isGoal(cellX, cellY))
            continue;

        Cell cell = cell_arr[cellX][cellY];

        if (cell.getHasBeenExplored())
        {

            minDistance = mazeSize * 2;
            int D1 = (!cell_arr[cellX][cellY].hasNorthWall()) ? cell_arr[cellX][cellY + 1].getFloodFillCost() : mazeSize * 2;
            int D2 = (!cell_arr[cellX][cellY].hasSouthWall()) ? cell_arr[cellX][cellY - 1].getFloodFillCost() : mazeSize * 2;
            int D3 = (!cell_arr[cellX][cellY].hasWestWall()) ? cell_arr[cellX - 1][cellY].getFloodFillCost() : mazeSize * 2;
            int D4 = (!cell_arr[cellX][cellY].hasEastWall()) ? cell_arr[cellX + 1][cellY].getFloodFillCost() : mazeSize * 2;
            minDistance = min(D1, D2);
            minDistance = min(minDistance, D3);
            minDistance = min(minDistance, D4);
            
            
            
            
            
            //----------add to stack

            if (cell_arr[cellX][cellY].getFloodFillCost() != 1 + minDistance)
            {
                cell_arr[cellX][cellY].setFloodFillCost(1 + minDistance);

                if (!cell_arr[cellX][cellY].hasNorthWall())
                {
                    floodStack.push(Coordinates{cellX, cellY + 1});
                }
                if (!cell_arr[cellX][cellY].hasSouthWall())
                {
                    floodStack.push(Coordinates{cellX, cellY - 1});
                }
                if (!cell_arr[cellX][cellY].hasWestWall())
                {
                    floodStack.push(Coordinates{cellX - 1, cellY});
                }
                if (!cell_arr[cellX][cellY].hasEastWall())
                {
                    floodStack.push(Coordinates{cellX + 1, cellY});
                }
            }
        }
        else
        {

            int minD = mazeSize * 2;
            // a cell that has not been explored has no walls so all neighbours are accessible

            int d1 = isSafe(cellX + 1, cellY) ? cell_arr[cellX + 1][cellY].getFloodFillCost() : mazeSize * 2;
            int d2 = isSafe(cellX - 1, cellY) ? cell_arr[cellX - 1][cellY].getFloodFillCost() : mazeSize * 2;
            int d3 = isSafe(cellX, cellY + 1) ? cell_arr[cellX][cellY + 1].getFloodFillCost() : mazeSize * 2;
            int d4 = isSafe(cellX, cellY - 1) ? cell_arr[cellX][cellY - 1].getFloodFillCost() : mazeSize * 2;
            
            
            
            
            minD = min(d1, d2);
            minD = min(minD, d3);
            minD = min(minD, d4);
            
            
            
            
            if (cell_arr[cellX][cellY].getFloodFillCost() != 1 + minD)
            {
                cell_arr[cellX][cellY].setFloodFillCost(1 + minD);

                if (isSafe(cellX + 1, cellY))
                {
                    floodStack.push(Coordinates{cellX + 1, cellY});
                }
                if (isSafe(cellX - 1, cellY))
                {
                    floodStack.push(Coordinates{cellX - 1, cellY});
                }
                if (isSafe(cellX, cellY + 1))
                {
                    floodStack.push(Coordinates{cellX, cellY + 1});
                }
                if (isSafe(cellX, cellY - 1))
                {
                    floodStack.push(Coordinates{cellX, cellY - 1});
                }
            }
        }
    }
}
bool isSafe(int X, int Y)
{
    if (X < 0 || X > mazeSize - 1)
        return false;
    if (Y < 0 || Y > mazeSize - 1)
        return false;
    return true;
}
