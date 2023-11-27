#include <vector>
#include<iostream>
#include<string>
#include<fstream>
#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include <chrono>
#include<unordered_map>
#include<iomanip>
#include<random>
#include<set>
using namespace std::chrono;
using namespace std;

struct Cell {
    int number;
    int x;
    int y;
    vector<int>nets;
};

class placer {
public:
    placer(string filename)
    {
        parseInput(filename, totalcomponents);
        totalHPWL = 0;
        auto start = high_resolution_clock::now();
        initialPlacement(totalcomponents);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
        calculateInitialHPWL();

    }

    void run() 
    {
        auto start = high_resolution_clock::now();
        annealing();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
        printFinalPlacement();   
    }

private:
    int numRows, numColumns;
    int totalcomponents, totalnets;
    vector<Cell*> components;
    vector<int> HPWL_X, HPWL_Y;
    vector<vector<Cell*>> netlist;
    vector<vector<Cell*>> netlist_y;
    vector<vector<int>> grid;
    int totalHPWL;

    void parseInput(const string filename, int& totalcomponents)
    {
        ifstream netfile(filename);
        if (!netfile.is_open())
        {
            cout << "File is not open" << endl;
            return;
        }
        netfile >> totalcomponents >> totalnets >> numRows >> numColumns;
        grid.resize(numRows);

        for (int i = 0; i < totalcomponents; i++)
        {
            Cell* temp = new Cell;
            temp->number = i;
            temp->x = -1;
            temp->y = -1;
            components.push_back(temp);
        }

        for (int i = 0; i < grid.size(); i++)
        {
            grid[i].resize(numColumns, -1);
        }

        netlist.reserve(totalnets);
        netlist_y.reserve(totalnets);

        for (int i = 0; i < totalnets; ++i) {
            int numberofcomponents;
            netfile >> numberofcomponents;
            vector<Cell*> net;
            net.reserve(numberofcomponents);
            for (int j = 0; j < numberofcomponents; j++)
            {
                int num;
                netfile >> num;
                components[num]->nets.push_back(i);
                Cell* temp = components[num];
                net.push_back(temp);
            }
            netlist.push_back(net);
            netlist_y.push_back(net);
        }
        netfile.close();
    }

 void initialPlacement(int numTotalComponents)
    {
        vector<int> cellFills(numTotalComponents);
        generate(cellFills.begin(), cellFills.end(), [n = 0] () mutable { return n++; });
        cellFills.resize(numRows*numColumns);

        for (int i = numTotalComponents; i <numRows*numColumns; i++)
        {
            cellFills[i] = -1;
        }

        auto rd = random_device {}; 
        auto rng = default_random_engine { rd() };
        shuffle(begin(cellFills), end(cellFills), rng);

        int x = 0;
        for (int i = 0; i < numRows; i++)
        {   for (int j = 0; j < numColumns; j++)
            {
                grid[i][j] = cellFills[x];
                if (cellFills[x] != -1 && cellFills[x] < numTotalComponents )
                    {
                        components[cellFills[x]]->x = i;
                        components[cellFills[x]]->y = j;                   
                    }
                x++;
            }
        }       
    }

    static bool compare_X_coordinate(Cell* A, Cell* B)
    {
        return A->x < B->x;
    }

    static bool compare_Y_coordinate(Cell* A, Cell* B)
    {
        return A->y < B->y;
    }

    int calculateHPWL_X(vector<Cell* >& X)
    {
        sort(X.begin(), X.end(), compare_X_coordinate);
        int dx = X.back()->x - X.front()->x;
        return dx;
    }
    int calculateHPWL_Y(vector<Cell* >& Y)
    {
        sort(Y.begin(), Y.end(), compare_Y_coordinate);
        int dy = Y.back()->y - Y.front()->y;
        return dy;
    }


    void calculateInitialHPWL()
    {
        for (int i = 0; i < netlist.size(); i++) {
            HPWL_X.push_back(calculateHPWL_X(netlist[i]));
            HPWL_Y.push_back(calculateHPWL_Y(netlist_y[i]));
        }
    
        for (int i=0; i < totalnets; i++) {
            totalHPWL += HPWL_X[i] + HPWL_Y[i];
        }
    }

    void checkHPWL_X(const Cell* cell, int net)
    {
        if (cell == netlist[net].front())
            {
            if (cell->x < netlist[net][1]->x)
            {
                totalHPWL -= HPWL_X[net];
                HPWL_X[net] = calculateHPWL_X(netlist[net]);
                totalHPWL += HPWL_X[net];
            }
            else {return;}
        }
        else if (cell == netlist[net].back())
        {
            if (cell->x > netlist[net][netlist[net].size()-2]->x)
            {
                totalHPWL -= HPWL_X[net];
                HPWL_X[net] = calculateHPWL_X(netlist[net]);
                totalHPWL += HPWL_X[net];
            }
            else {return;}
        }
        else
        {
            if (cell->x > netlist[net].back()->x || cell->x < netlist[net].front()->x)
            {
                totalHPWL -= HPWL_X[net];
                HPWL_X[net] = calculateHPWL_X(netlist[net]);
                totalHPWL += HPWL_X[net];
            }
            else {return;}
        }
    }

    void checkHPWL_Y(const Cell* cell, int net)
    {
        if (cell == netlist_y[net].front())
            {
            if (cell->y < netlist_y[net][1]->y)
            {
                totalHPWL -= HPWL_Y[net];
                HPWL_Y[net] = calculateHPWL_Y(netlist_y[net]);
                totalHPWL += HPWL_Y[net];
            }
            else {return;}
        }
        else if (cell == netlist_y[net].back())
        {
            if (cell->y > netlist_y[net][netlist_y[net].size()-2]->y)
            {
                totalHPWL -= HPWL_Y[net];
                HPWL_Y[net] = calculateHPWL_Y(netlist_y[net]);
                totalHPWL += HPWL_Y[net];
            }
            else {return;}
        }
        else
        {
            if (cell->y> netlist_y[net].back()->y || cell->y < netlist_y[net].front()->y)
            {
                totalHPWL -= HPWL_Y[net];
                HPWL_Y[net] = calculateHPWL_Y(netlist_y[net]);
                totalHPWL += HPWL_Y[net];
            }
            else {return;}
        }
    }

    bool checker(const  Cell* X, const Cell* Y, double temperature)
    {
        if (X != NULL)
        {
            for (int i = 0; i < X->nets.size(); i++)
            {
                checkHPWL_X (X, X->nets[i]);
                checkHPWL_Y (X, X->nets[i]);
            }
        }

        if (Y != NULL)
        {   for (int i = 0; i < Y->nets.size(); i++)
            {
                checkHPWL_X (Y, Y->nets[i]);
                checkHPWL_Y (Y, Y->nets[i]);
            }
        }

        return totalHPWL < temperature;
    }



    void annealing() {
        cout << "started annealing" << endl;
        double initialCost = totalHPWL;
        double initialTemp = initialCost * 500;
        double finalTemp = 5 * pow(10, -6) * initialCost/ totalnets;
        double currentTemp = initialTemp;

        srand(time(0));
        while (currentTemp > finalTemp) {
            for (int i = 0; i < 10 * totalcomponents; ++i) {

                int x_temp1 = rand() % numRows;
                int x_temp2 = rand() % numRows;
                int y_temp1 = rand() % numColumns;
                int y_temp2 = rand() % numColumns;

                if (x_temp1 == x_temp2 && y_temp1 == y_temp2) {
                    continue;
                }

                Cell* X = NULL;
                Cell* Y = NULL;

                int comp1 = grid[x_temp1][y_temp1];
                int comp2 = grid[x_temp2][y_temp2];
                if (comp1 != -1)
                {
                    components[comp1]->x = x_temp2;
                    components[comp1]->y = y_temp2;
                    X = components[comp1];
                }
                if (comp2 != -1)
                {
                    components[comp2]->x = x_temp1;
                    components[comp2]->y = y_temp1;
                    Y = components[comp2];
                }
                if (checker(X, Y, currentTemp)) {
                    grid[x_temp1][y_temp1] = comp2;
                    grid[x_temp2][y_temp2] = comp1;
                }
                else
                {
                    if (comp1 != -1)
                    {
                        components[comp1]->x = x_temp1;
                        components[comp1]->y = y_temp1;
                    }
                    if (comp2 != -1)
                    {
                        components[comp2]->x = x_temp2;
                        components[comp2]->y = y_temp2;
                    }                    
                }
            }

            currentTemp *= 0.95;
        }

        cout << "Initial cost: " << initialCost << endl;
        cout << "Final Cost: " << totalHPWL << endl;
    }

    void printFinalPlacement() const
    {
        for (const auto& row : grid) {
            for (int cell : row) {
                if (cell == -1) {
                    cout << "----" << "\t";
                }
                else {
                    cout << setw(4) << setfill('0') << cell << "\t";
                }
            }
            cout << endl;
        }
    }
};

void emitError(char *s)
{
    cout << s;
    exit(0);
}

int main(int argc, char *argv[]) {
        
    if(argc<2) emitError("use: placer <netlist_file_name>\n");
    placer place(argv[1]);
    place.run();
    return 0;
}