#include <iostream>
#include <random>
#include <string>

using namespace std;

//Create a 4x4 grid filled with 0
vector<vector<int>> grid(4, vector<int>(4, 0));
bool alive = true;

enum Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        INVALID
    };
Direction currentDirection;

random_device r;
seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
mt19937 e1(seed);

class PlaceNewNumber {
    private:
    static int generateNumber() {
        uniform_int_distribution<int> uniform_dist(1, 10);
        //10% chance of generating a 4
        if (uniform_dist(e1) == 1) {
            return 4;
        }
        else {
            return 2;
        }
    }

    public:
    static vector<int> getFreeIndexes() {
        vector<int> freeIndexes;
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 4; column++) {
                if (grid[row][column] == 0) {
                    freeIndexes.push_back(4*row + column);
                }
            }
        }
        return freeIndexes;
    }

    private:
    static int selectIndex() {
        vector<int> freeIndexes = getFreeIndexes();
        if (freeIndexes.size() == 0) return -1;
        uniform_int_distribution<int> uniform_dist(0, freeIndexes.size()-1);
        return freeIndexes[uniform_dist(e1)];
    }

    public:
    static void placeNewNumber() {
        int index = selectIndex();
        if (index == -1) return;
        int number = generateNumber();
        grid[index/4][index%4] = number;
    }
};
class ShowGrid {
    public:
    static void showGrid() {
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 4; column++) {
                if (grid[row][column] == 0) {
                    cout << ". ";
                }
                else {
                    cout << grid[row][column] << " ";
                }
            }
            cout << endl;
        }
    }
};
class Alive {
    private:
    static bool anyFreeIndexes() {
        if (PlaceNewNumber::getFreeIndexes().size() > 0) return true;
        return false;
    }

    static bool canNumbersMerge() {
        //Test horizontal
        for (int row = 0; row < 4; row++) {
            for (int column = 0; column < 3; column++) {
                if (grid[row][column] == grid[row][column+1]) {
                    return true;
                }
            }
        }
        //Test vertical
        for (int row = 0; row < 3; row++) {
            for (int column = 0; column < 4; column++) {
                if (grid[row][column] == grid[row+1][column]) {
                    return true;
                }
            }
        }
        //If all tests fail return false
        return false;
    }

    public:
    static bool alive() {
        if (anyFreeIndexes()) return true;
        //If the numbers can merge return true, otherwise return false
        return canNumbersMerge();
    }
};
class GetInput {
    private:
    static void setDirection(string input) {
        if (input == "w") currentDirection = UP;
        else if (input == "s") currentDirection = DOWN;
        else if (input == "a") currentDirection = LEFT;
        else if (input == "d") currentDirection = RIGHT;
        else currentDirection = INVALID;
    }

    public:
    static void getInput() {
        string input;
        getline(cin, input);
        setDirection(input);
        if (currentDirection == INVALID) {
            cout << "Enter w, a, s, or d\n"
            getInput();
        }
    }
};
class MoveNumbers {
    private:
    //DOWN = +verticalLookDirection
    static int verticalMoveDirection;
    static int startRow;
    static int rowIncrement;

    //RIGHT = +horizontalLookDirection
    static int horizontalMoveDirection;
    static int startColumn;
    static int columnIncrement;

    static void setDefaults() {
        verticalMoveDirection = 0;
        startRow = 0;
        rowIncrement = 1;

        horizontalMoveDirection = 0;
        startColumn = 0;
        columnIncrement = 1;
    }

    static void setVariables() {
        setDefaults();
        switch (currentDirection) {
            case UP:
                verticalMoveDirection = -1;
                startRow = 1;
                rowIncrement = 1;
                break;
            case DOWN:
                verticalMoveDirection = 1;
                startRow = 2;
                rowIncrement = -1;
                break;
            case LEFT:
                horizontalMoveDirection = -1;
                startColumn = 1;
                columnIncrement = 1;
                break;
            case RIGHT:
                horizontalMoveDirection = 1;
                startColumn = 2;
                columnIncrement = -1;
                break;
            case INVALID:
                throw "what";
                break;
        }
    }

    static bool outOfBounds(int number) {
        return number < 0 || number > 3;
    }

    static void moveOne(int row, int column) {
        bool hasMoved = false;
        if (verticalMoveDirection != 0) { //If supposed to look vertically
            int lookingAtRow;
            for (lookingAtRow = row+verticalMoveDirection; !outOfBounds(lookingAtRow); lookingAtRow+=verticalMoveDirection) { //Look through rows until it's out of bounds
                if (grid[lookingAtRow][column] != 0) { //If it sees a tile
                    grid[lookingAtRow-verticalMoveDirection][column] = grid[row][column]; //Move to previous tile that was free
                    grid[row][column] = 0; //Make the square that used to have the tile 0
                    hasMoved = true;
                    break;
                }
            }
            if (!hasMoved) {
                grid[lookingAtRow-verticalMoveDirection][column] = grid[row][column]; //Move to previous tile that was free
                grid[row][column] = 0; //Make the square that used to have the tile 0
            }
        }
        
        else if (horizontalMoveDirection != 0) {
            int lookingAtColumn;
            for (lookingAtColumn = column+horizontalMoveDirection; !outOfBounds(lookingAtColumn); lookingAtColumn+=horizontalMoveDirection) { //Look through rows until it's out of bounds
                if (grid[row][lookingAtColumn] != 0) { //If it sees a tile
                    grid[row][lookingAtColumn-horizontalMoveDirection] = grid[row][column]; //Move to previous tile that was free
                    grid[row][column] = 0; //Make the square that used to have the tile 0
                    hasMoved = true;
                    break;
                }
            }
            if (!hasMoved) {
                grid[row][lookingAtColumn-horizontalMoveDirection] = grid[row][column]; //Move to previous tile that was free
                grid[row][column] = 0; //Make the square that used to have the tile 0
            }
        }
            
        else {
            throw "what2";
        }
    }

    static void moveEach() {
        for (int row = startRow; !outOfBounds(row); row+=rowIncrement) {
            for (int column = startColumn; !outOfBounds(column); column+=columnIncrement) {
                //If current tile is not 0
                if (grid[row][column] != 0) {
                    moveOne(row, column);
                }
            }
        }
    }

    public:
    static void moveNumbers() {
        //Set variables based on input
        setVariables();
        //Merge t
        moveEEach();ch();
    }
};

int main() {
    cout << "Enter w, a, s, or d to move numbers\n";
    while (alive) {
        PlaceNewNumber::placeNewNumber();
        ShowGrid::showGrid();
        if (!Alive::alive()) {
            break;
        }
        GetInput::getInput();
        
        MoveNumbers::moveNumbers();
    }
    return 0;
}