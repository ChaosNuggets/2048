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

vector<int> freeIndexes; //Can be updated using PlaceNewNumber::getFreeIndexes()

int row;
int column;
void forEach(void (*doThing)(), int rowEnd, int columnEnd) {
    for (row = 0; row < rowEnd; row++) {
        for (column = 0; column < columnEnd; column++) {
            doThing();
        }
    }
}
void forEach(void (*doThing)()) {
    forEach(doThing, 4, 4);
}

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

    static void testIfFree() {
        if (grid[row][column] == 0) {
            freeIndexes.push_back(4*row + column);
        }
    }

    static void getFreeIndexes() {
        forEach(testIfFree);
    }

    static int selectIndex() {
        getFreeIndexes();
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
    private:
    static void showNumber() {
        if (grid[row][column] == 0) {
            cout << ". ";
        }
        else {
            cout << grid[row][column] << " ";
        }
        if (column == 3) {
            cout << endl;
        }
    }
    public:
    static void showGrid() {
        forEach(showNumber);
    }
};
class Alive {
    private:
    static bool anyFreeIndexes() {
        //One of the free indexes has been filled in
        if (freeIndexes.size() > 1) return true;
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
class MergeAndMoveNumbers {
    private:
    static void leftMerge() {
        for (int checkingColumn = column + 1; checkingColumn < 4; checkingColumn++) {
            if (grid[row][checkingColumn] != 0) {
                //If can merge
                if (grid[row][checkingColumn] == grid[row][column]) {
                    grid[row][column] *= 2; //Double left tile
                    grid[row][checkingColumn] = 0; //Destroy right tile
                    column = checkingColumn + 1; //We only need to check past the tile we have just destroyed
                }
                break;
            }
        }
    }
    static vector<int> amountErased; //Index 0 stores # erased in row 0
    static void leftMove() {
        if (grid[row][column] == 0) {
            grid[row].erase(grid[row].begin() + column);
            
        }
    }
    static void doLeft() {
        forEach(leftMerge)
    }

    public:
    static void mergeAndMoveNumbers() {
        switch (currentDirection) {
            case UP:
                doUp();
                break;
            case DOWN:
                doDown();
                break;
            case LEFT:
                doLeft();
                break;
            case RIGHT:
                doRight();
                break;
            case INVALID:
                throw "what2";
                break;
        }
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
        
        MergeAndMoveNumbers::mergeAndMoveNumbers();
    }
    return 0;
}