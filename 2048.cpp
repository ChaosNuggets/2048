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
int amountRemoved; //Used in move functions
int score;

int row;
int column;
bool doBreak;
void forEach(void (*doThing)(), int rowEnd, int columnEnd) {
    doBreak = false;
    for (row = 0; row < rowEnd; row++) {
        for (column = 0; column < columnEnd; column++) {
            doThing();
            if (doBreak) break;
        }
        if (doBreak) break;
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
        freeIndexes.clear();
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
class IsThere2048 {
    private:
    static void testFor2048() {
        if (grid[row][column] == 2048) doBreak = true;
    }
    public:
    static bool isThere2048() {
        forEach(testFor2048);
        //If it broke out of the loop(if it found 2048), return true
        if (doBreak) return true;
        return false;
    }
    static void sendCongratulationMsg() {
        cout << "Congratulations! You won!\n";
        cout << "Press enter to continue playing";
        cin.ignore();
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
            cout << "Enter w, a, s, or d\n";
            getInput();
        }
    }
};
class DoDirection {
    public:
    void merge(void (*func)()) {
        
    }

    void move(void (*func)()) {
        
    }
}; //TODO: finish this
class DoUp {
    //Everything's inverted here so we can stil use the forEach function. We are now iterating through the grid from top to bottom, then move one column right. Column = row, and row = column. All the other variables are not inverted.
    private:
    static void upMerge() {
        for (int checkingRow = column + 1; checkingRow < 4; checkingRow++) {
            if (grid[checkingRow][row] != 0) {
                //If can merge
                if (grid[checkingRow][row] == grid[column][row]) {
                    grid[column][row] *= 2; //Double upper tile
                    grid[checkingRow][row] = 0; //Destroy lower tile
                    column = checkingRow + 1; //We only need to check past the tile we have just destroyed
                }
                break;
            }
        }
    }
    static void upMove() {
        int checkingRow = column-amountRemoved;
        if (grid[checkingRow][row] == 0) {
            //Move everything in the row
            for (int i = checkingRow; i <= 2; i++) {
                grid[i][row] = grid[i+1][row];
            }
            grid[3][row] = 0; //Make lowest tile in the row 0
            amountRemoved++;
        }
        if (column == 2) {
            amountRemoved = 0;
        }
    }
    public:
    static void doUp() {
        amountRemoved = 0;
        forEach(upMerge, 4, 3);
        forEach(upMove, 4, 3);
    }
};
class DoDown {
    //Everything's inverted here so we can stil use the forEach function. We are now iterating through the grid from bottom to top, then move one column right. Column = row, and row = column. All the other variables are not inverted.
    private:
    static void downMerge() {
        int currentRow = 3-column; //Invert row (now we're looking from bottom to top)
        for (int checkingRow = currentRow - 1; checkingRow >= 0; checkingRow--) {
            if (grid[checkingRow][row] != 0) {
                //If can merge
                if (grid[checkingRow][row] == grid[currentRow][row]) {
                    grid[currentRow][row] *= 2; //Double right tile
                    grid[checkingRow][row] = 0; //Destroy right tile
                    column = checkingRow + 1; //We only need to check past the tile we have just destroyed
                }
                break;
            }
        }
    }
    static void downMove() {
        int checkingRow = 3-column+amountRemoved; //Invert row (now we're going from bottom to top)
        if (grid[checkingRow][row] == 0) {
            //Move everything in the row
            for (int i = checkingRow; i >= 1; i--) {
                grid[i][row] = grid[i-1][row];
            }
            grid[0][row] = 0; //Make the top tile in the column 0
            amountRemoved++;
        }
        if (column == 2) {
            amountRemoved = 0;
        }
    }
    public:
    static void doDown() {
        amountRemoved = 0;
        forEach(downMerge, 4, 3);
        forEach(downMove, 4, 3);
    }
};
class DoLeft {
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
    static void leftMove() {
        int checkingColumn = column-amountRemoved;
        if (grid[row][checkingColumn] == 0) {
            //Move everything in the column
            for (int i = checkingColumn; i <= 2; i++) {
                grid[row][i] = grid[row][i+1];
            }
            grid[row][3] = 0; //Make rightmost tile in the row 0
            amountRemoved++;
        }
        if (column == 2) {
            amountRemoved = 0;
        }
    }
    public:
    static void doLeft() {
        amountRemoved = 0;
        forEach(leftMerge, 4, 3);
        forEach(leftMove, 4, 3);
    }
};
class DoRight {
    private:
    static void rightMerge() {
        int currentColumn = 3-column; //Invert column (now we're looking from right to left)
        for (int checkingColumn = currentColumn - 1; checkingColumn >= 0; checkingColumn--) {
            if (grid[row][checkingColumn] != 0) {
                //If can merge
                if (grid[row][checkingColumn] == grid[row][currentColumn]) {
                    grid[row][currentColumn] *= 2; //Double right tile
                    grid[row][checkingColumn] = 0; //Destroy right tile
                    column = checkingColumn + 1; //We only need to check past the tile we have just destroyed
                }
                break;
            }
        }
    }
    static void rightMove() {
        int checkingColumn = 3-column+amountRemoved; //Invert column (now we're going from right to left)
        if (grid[row][checkingColumn] == 0) {
            //Move everything in the column
            for (int i = checkingColumn; i >= 1; i--) {
                grid[row][i] = grid[row][i-1];
            }
            grid[row][0] = 0; //Make the leftmost tile in the row 0
            amountRemoved++;
        }
        if (column == 2) {
            amountRemoved = 0;
        }
    }
    public:
    static void doRight() {
        amountRemoved = 0;
        forEach(rightMerge, 4, 3);
        forEach(rightMove, 4, 3);       
    }
};
class MergeAndMoveNumbers {
    public:
    static void mergeAndMoveNumbers() {
        switch (currentDirection) {
            case UP:
                DoUp::doUp();
                break;
            case DOWN:
                DoDown::doDown();
                break;
            case LEFT:
                DoLeft::doLeft();
                break;
            case RIGHT:
                DoRight::doRight();
                break;
            case INVALID:
                throw "what2";
                break;
        }
    }
};
class CalculateScore {
    private:
    static void addToScore() {
        score += grid[row][column];
    }
    public:
    static void calculateScore() {
        score = 0;
        forEach(addToScore);
        cout << "Game over, score: " << score << endl;
    }
};

int main() {
    bool hasWon = false;
    cout << "\033[2J\033[0;0H"; //Clear screen
    cout << "Enter w, a, s, or d to move numbers\n";
    while (alive) {
        PlaceNewNumber::placeNewNumber();
        ShowGrid::showGrid();
        if (hasWon = false && IsThere2048::isThere2048()) {
            IsThere2048::sendCongratulationMsg();
        }
        if (!Alive::alive()) {
            break;
        }
        vector<vector<int>> prevGrid = grid;
        //Keep the player in this loop until they actually move/merge something
        while (grid == prevGrid) {
            GetInput::getInput();
            MergeAndMoveNumbers::mergeAndMoveNumbers();
        }
        cout << "\033[2J\033[0;0H"; //Clear screen
    }
    CalculateScore::calculateScore();
    return 0;
}