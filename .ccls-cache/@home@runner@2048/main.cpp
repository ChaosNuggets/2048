#include <iostream>
#include <cmath>
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
            if (doBreak) return;
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
            return 14;
        }
        else {
            return 7;
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
            cout << ".     ";
        }
        else {
            cout << grid[row][column] << " ";
            //Make it so that the numbers line up vertically
            int numberOfDigits = floor(log10(static_cast<float>(grid[row][column])));
            for (float i = 0; i < 4-numberOfDigits; i++) {
                cout << ' ';
            }
        }
        if (column == 3) {
            cout << "\n\n";
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
        cout << "Press enter to continue playing\n";
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

    static void testHorizontal() {
        if (grid[row][column] == grid[row][column+1]) {
            doBreak = true;
        }
    }

    static void testVertical() {
        if (grid[row][column] == grid[row+1][column]) {
            doBreak = true;
        }
    }

    static bool canNumbersMerge() {
        forEach(testHorizontal, 4, 3);
        if (doBreak) return true;

        forEach(testVertical, 3, 4);
        //If this test passed return true, otherwise return false
        return doBreak;
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
    //merge returns true if it should break out of the for loop
    static bool merge(int currentRow, int currentColumn, int checkingRow, int checkingColumn, bool inverted) {
        if (grid[checkingRow][checkingColumn] != 0) {
            //If can merge
            if (grid[checkingRow][checkingColumn] == grid[currentRow][currentColumn]) {
                grid[currentRow][currentColumn] *= 2; //Double upper tile
                grid[checkingRow][checkingColumn] = 0; //Destroy lower tile
                if (inverted) {
                    column = checkingRow + 1; //We only need to check past the tile we have just destroyed
                }
                else {
                    column = checkingColumn + 1;
                }
            }
            return true;
        }
        return false;
    }

    static void mergeAndMove(void (*mergeFunc)(), void (*moveFunc)()) {
        amountRemoved = 0;
        forEach(mergeFunc, 4, 3);
        forEach(moveFunc, 4, 3);
    }
};
class DoUp : DoDirection {
    //Everything's inverted here so we can stil use the forEach function. We are now iterating through the grid from top to bottom, then move one column right. Column = row, and row = column. All the other variables are not inverted.
    private:
    static void upMerge() {
        for (int checkingRow = column + 1; checkingRow < 4; checkingRow++) {
            //merge returns true if it should break out of the for loop
            if (merge(column, row, checkingRow, row, true)) {
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
        mergeAndMove(upMerge, upMove);
    }
};
class DoDown : DoDirection {
    //Everything's inverted here so we can stil use the forEach function. We are now iterating through the grid from bottom to top, then move one column right. Column = row, and row = column. All the other variables are not inverted.
    private:
    static void downMerge() {
        int currentRow = 3-column; //Invert row (now we're looking from bottom to top)
        for (int checkingRow = currentRow - 1; checkingRow >= 0; checkingRow--) {
            if (merge(currentRow, row, checkingRow, row, true)) {
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
        mergeAndMove(downMerge, downMove);
    }
};
class DoLeft : DoDirection {
    private:
    static void leftMerge() {
        for (int checkingColumn = column + 1; checkingColumn < 4; checkingColumn++) {
            if (merge(row, column, row, checkingColumn, false)) {
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
        mergeAndMove(leftMerge, leftMove);
    }
};
class DoRight : DoDirection {
    private:
    static void rightMerge() {
        int currentColumn = 3-column; //Invert column (now we're looking from right to left)
        for (int checkingColumn = currentColumn - 1; checkingColumn >= 0; checkingColumn--) {
            if (merge(row, currentColumn, row, checkingColumn, false)) {
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
        mergeAndMove(rightMerge, rightMove);    
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
        cout << "Press enter to exit\n";
        cin.ignore();
    }
};

int main() {
    bool hasWon = false;
    cout << "Enter w, a, s, or d to move numbers\n";
    while (alive) {
        PlaceNewNumber::placeNewNumber();
        ShowGrid::showGrid();
        if (hasWon == false && IsThere2048::isThere2048()) {
            IsThere2048::sendCongratulationMsg();
            hasWon = true;
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
        cout << "\u001b[2J"; //Clear screen
    }
    CalculateScore::calculateScore();
    return 0;
}