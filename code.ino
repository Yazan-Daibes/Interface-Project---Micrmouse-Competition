#include "ArduinoSTL.h"
#include "Cell.h"
#include "stack"
#include "TimerOne.h"

const int IN1 = 9; // right motor
const int IN2 = 10; // right motor
const int IN3 = 7; // left motor
const int IN4 = 8; // left motor
const byte MOTOR_A = 3;  // Motor 2 Interrupt Pin - INT 1 - left Motor
const byte MOTOR_B = 2;  // Motor 1 Interrupt Pin - INT 0 - right Motor
const int ENA = 5;
const int ENB = 6;
// Constant for steps in disk
const float stepcount = 20.00;  // 20 Slots in disk
 
// Constant for wheel diameter
const float wheeldiameter = 33; // Wheel diameter in millimeters
 
// Integers for pulse counter
volatile int counter_A = 0;
volatile int counter_B = 0;

// pins
const byte trigPinF=A1;
const byte echoPinF=A0;
const byte trigPinL=A3;
const byte echoPinL=A2;
const byte trigPinR=A5;
const byte echoPinR=A4;

long durF;
int distF;
long durR;
int distR;
long durL;
int distL;
int flag;


int mazeSize = 8; // 8X8
int centerX = mazeSize / 2;
int centerY = mazeSize / 2;

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
 int CMtoSteps(float cm);
 void ISR_timerone();
 void ISR_countA();
 void ISR_countB();
 //void  Stop();
//----setup---
void setup() {
  Serial.begin(9600);
  pinMode(trigPinF,OUTPUT);
  pinMode(echoPinF,INPUT);
  pinMode(trigPinR,OUTPUT);
  pinMode(echoPinR,INPUT);
  pinMode(trigPinL,OUTPUT);
  pinMode(echoPinL,INPUT);

  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);

  pinMode (ENA, OUTPUT);
  pinMode (ENB, OUTPUT);

//  analogWrite(PWMPin, 124);
//  analogWrite(PWMPin2, 130);
 
  attachInterrupt(digitalPinToInterrupt (MOTOR_A), ISR_countA, RISING);  // Increase counter A when speed sensor pin goes High
  attachInterrupt(digitalPinToInterrupt (MOTOR_B), ISR_countB, RISING);  // Increase counter B when speed sensor pin goes High

 flag = 0;

}

void loop(){
  
     if (!flag){
        Cell cell_arr [8][8];    
        initializeMaze(cell_arr);
        findGoal(cell_arr);
        Serial.println("3");
        flag=1;
        delay(10000);
    }
    exit(0);
}


//---------------------- function defintions-------------------------------

bool wallFront()
{
  digitalWrite(trigPinF,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinF,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPinF,LOW);
  durF=pulseIn(echoPinF,HIGH);
  distF=(durF/2) / 29.1;
  if(distF<20)
    return true;
  else 
    return false;
   
}


bool wallRight()
{
  digitalWrite(trigPinR,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinR,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPinR,LOW);
  durR=pulseIn(echoPinR,HIGH);
  distR=(durR/2) / 29.1;
  if(distR<20)
    return true;
  else
    return false;
}
bool wallLeft()
{
    
  digitalWrite(trigPinL,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPinL,HIGH);
  delayMicroseconds(2);
  digitalWrite(trigPinL,LOW);
  durL=pulseIn(echoPinL,HIGH);
  distL=(durL/2) / 29.1;
  if(distL<20)
    return true;
  else 
    return false;
  
}

///
void MoveForward(int steps, int mspeed1,int mspeed2) {
counter_A = 0;  //  reset counter A to zero
   counter_B = 0;  //  reset counter B to zero
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4, LOW);
// Go forward until step value is reached
   while (steps > counter_A &&  steps > counter_B) {
    if (steps > counter_A) {
    analogWrite(ENA, mspeed1);
    } else {
    analogWrite(ENA, 0);
    }
    if (steps > counter_B) {
    analogWrite(ENB, mspeed2);
    } else {
    analogWrite(ENB, 0);
    }
   }
    
  // Stop when done
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero 
 
}
void turnRight(int steps, int mspeed) {
counter_A = 0;  //  reset counter A to zero
   counter_B = 0;  //  reset counter B to zero
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
 digitalWrite(IN3, LOW);
 digitalWrite(IN4, HIGH);
// Go forward until step value is reached
   while (steps > counter_A &&  steps > counter_B) {

    if (steps > counter_A) {
    analogWrite(ENA, mspeed);
    } else {
    analogWrite(ENA, 0);
    }
    if (steps > counter_B) {
    analogWrite(ENB, mspeed);
    } else {
    analogWrite(ENB, 0);
    }
   }
    
  // Stop when done
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero 
  }
void turnLeft(int steps, int mspeed) {
counter_A = 0;  //  reset counter A to zero
   counter_B = 0;  //  reset counter B to zero
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
 digitalWrite(IN3, HIGH);
 digitalWrite(IN4, LOW);
// Go forward until step value is reached
   while (steps > counter_A &&  steps > counter_B) {
    
    if (steps > counter_A) {
    analogWrite(ENA, mspeed);
    } else {
    analogWrite(ENA, 0);
    }
    if (steps > counter_B) {
    analogWrite(ENB, mspeed);
    } else {
    analogWrite(ENB, 0);
    }
   }
    
  // Stop when done
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero 
 
}

void Stop(){

  digitalWrite(IN1,0);
  digitalWrite(IN2,0);
  digitalWrite(IN3,0);
  digitalWrite(IN4,0);
  delay(300);
}

bool isGoal(int x, int y)
{
//    return ((x == 3 || x == 4) && (y == 3 || y == 4));
 if(x==3 && y==3){
    return true;
   }
   else if(x==3 && y==4){
    return true;
   }
   else if(x==4 && y==3){
    return true;
   }
   else if(x==4 && y==4){
    return true;
   }
  else {
    return false;
  }
}



void updateposition(char currentMove)
{
//   Serial.println("updateposition");
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
//    Serial.println("Explore Cell");
    // map[x][y].setVisited(); // first visit is straight, then right, then left, then back
    if (cell_arr[x][y].getHasBeenExplored())
        return;

    cell_arr[x][y].sethasBeenExplored(true);

    switch (orientation)
    {
    case 'N':
//        Serial.println("N");
        cell_arr[x][y].setNorthWall(wallFront());
        cell_arr[x][y].setEastWall(wallRight());
        cell_arr[x][y].setWestWall(wallLeft());

        break;
    case 'S':
//        Serial.println("S");
        cell_arr[x][y].setSouthWall(wallFront());
        cell_arr[x][y].setEastWall(wallLeft());
        cell_arr[x][y].setWestWall(wallRight());
        break;
    case 'W':
//         Serial.println("W");
        cell_arr[x][y].setWestWall(wallFront());
        cell_arr[x][y].setSouthWall(wallLeft());
        cell_arr[x][y].setNorthWall(wallRight());
        break;
    case 'E':
//         Serial.println("E");
        cell_arr[x][y].setEastWall(wallFront());
        cell_arr[x][y].setNorthWall(wallLeft());
        cell_arr[x][y].setSouthWall(wallRight());
        break;
    default:
        int a = 0;
    }
}

void initializeMaze(Cell cell_arr[][8])
{

    for (int i = 0; i < mazeSize; i++)
    {
        for (int j = 0; j < mazeSize; j++)
        {
            cell_arr[i][j].setcellAddress(Coordinates{i, j});
            int minX = min(abs(i - centerX), abs(i - (centerX - 1)));
            int minY = min(abs(j - centerY), abs(j - (centerY - 1)));
            int floodFillCost = minX + minY;

            cell_arr[i][j].setFloodFillCost(floodFillCost);
    }

    cell_arr[0][0].setSouthWall(true); 
}
}

void findGoal(Cell cell_arr[][8] )  // takes coordinates of current cell
{
    bool destinationFound = false;
    int minDistance;
    char minDirection;
    // alias for conveneience
    while (!destinationFound) // !destinationFound
    {

        exploreCell(cell_arr, x, y); // explore current cell
        minDistance = mazeSize * 2;

//                Serial.print("x : ");
//        Serial.println(x);
//        Serial.print("Y : ");
//        Serial.println(y);
        // if goal fount-> exit while loop
        Serial.print("X = ");
        Serial.println(x);
        Serial.print("Y = ");
        Serial.println(y);
        
        if (isGoal(x, y))
           {
             destinationFound = true;
             Serial.println("Is Goal == > break");
             break;
           }

        getMinDistanceDirection(cell_arr, minDistance, minDirection);
        // check if reflooding is required

        if (cell_arr[x][y].getFloodFillCost() != 1 + minDistance)
        { 

            // reflood
            floodOpenNeighbours(cell_arr);

            // after reflooding get new min distance neighbours
            getMinDistanceDirection(cell_arr, minDistance, minDirection); //get neighbour with lowest distance
            delay(500); //imp
        }

//             Serial.println("here22222222222222222222222222222222222222222");

        moveInDirection(cell_arr, minDirection);
      
    }
        Serial.println("here22222222222222222222222222222222222222222");
//  delay(100);
}

void moveInDirection(Cell cell_arr[][8], char direction)
{

    // move to the  neighbouring cell with the lowest distance cost
    int prevX = x, prevY = y;
    if (direction == 'f')
    { 
        MoveForward(250,122,130);
        updateposition('f');
        Serial.println("forward");
        Serial.println("=============================");
        delay(1000);
    }
    else if (direction == 'l')
    {
        turnLeft(70,100);
        updateposition('l');
        MoveForward(250,122,130);
        updateposition('f');
        Serial.println("left");
        Serial.println("============================="); 
        delay(1000);
    }

    else if (direction == 'r')
    {
        turnRight(70,100);

        updateposition('r');
        MoveForward(250,122,130);
 
        updateposition('f');
         Serial.println("right");
          Serial.println("=============================");
          delay(1000);
    }
    else if (direction == 'b')
    {
        
         turnRight(70,100);
         updateposition('r');
         turnRight(70,100);
         updateposition('r');
         
         MoveForward(250,122,130);
         updateposition('f');
        Serial.println("backward");
         Serial.println("=============================");
         delay(1000);
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

    if (wallFront()== false )
    {
        Serial.println("wall front = false");

        Cell front = getFrontCell(cell_arr);


        if (minDistance > front.getFloodFillCost())
        {
            minDistance = front.getFloodFillCost();
            minDirection = 'f';
        }
    }
    if (wallLeft() == false)
    {
        Serial.println("wall left = false");
        Cell left = getLeftCell(cell_arr);
//        Serial.println("After get left ");
        if (minDistance > left.getFloodFillCost())
        {
            minDistance = left.getFloodFillCost();
            minDirection = 'l';
        }
    }
    if (wallRight()== false)
    {
        Serial.println("wall right = false");

        Cell right = getRightCell(cell_arr);
//        Serial.println("After get right ");
        if (minDistance > right.getFloodFillCost())
        {
            minDistance = right.getFloodFillCost();
            minDirection = 'r';
        }
    }
    if (cell_arr[x][y].getPrevVisitedCell() != NULL)
    {
        Serial.println("in else ");
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

    stack<Coordinates> floodStack; // the taken path
    int minDistance = mazeSize * 2;
    char minDirection;
    int cellX, cellY;
    
    floodStack.push(Coordinates({x, y}));
    while (!floodStack.empty())
    {
        cellX = floodStack.top().x;
        cellY = floodStack.top().y;
//        Serial.print("x : ");
//        Serial.println(cellX);
//        Serial.print("Y : ");
//        Serial.println(cellY);
                
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
//             Serial.print("hasNorthWall() = ");
//             Serial.println(cell_arr[cellX][cellY].hasNorthWall());
//             Serial.print("hasSouthWall() = ");
//             Serial.println(cell_arr[cellX][cellY].hasSouthWall());
//             Serial.print("hasWestWall() = ");
//             Serial.println(cell_arr[cellX][cellY].hasWestWall());
//             Serial.print("hasEastWall() = ");
//             Serial.println(cell_arr[cellX][cellY].hasEastWall());
//             Serial.println("===============================================================");
//             delay(1000);
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

void ISR_countA()  
{
  counter_A++;  // increment Motor A counter value
} 
 
// Motor B pulse count ISR
void ISR_countB()  
{
  counter_B++;  // increment Motor B counter value
}
