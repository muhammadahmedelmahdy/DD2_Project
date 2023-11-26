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
        // int i = 1;
        // for (const auto& net : netlist) 
        // {
        //     cout << "Net: " << i++ << " "; 
        //     for (const auto& cell : net) 
        //         cout<< "Com: " << cell->number << ", X: " << cell->x << ", Y: " << cell->y << " \t";
        //     cout << endl;
        // }       
    }

private:
    int numRows, numColumns;
    int totalcomponents, totalnets;
    vector<Cell*> components;
    vector<int> HPWL;
    vector<vector<Cell*>> netlist;
    vector<vector<int>> grid;

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

    int calculateHPWL(const vector<Cell* >& net)
    {
        vector<Cell*> X(net.begin(), net.end());
        vector<Cell*> Y(net.begin(), net.end());
        sort(X.begin(), X.end(), compare_X_coordinate);
        sort(Y.begin(), Y.end(), compare_Y_coordinate);
        int dx = X.back()->x - X.front()->x;
        int dy = Y.back()->y - Y.front()->y;
        return dx + dy;
    }

    void calculateInitialHPWL()
    {
        for (auto const& net : netlist) {
            HPWL.push_back(calculateHPWL(net));
        }
    }

    int calculateTotalHPWL()
    {
        int total = 0;
        for (int i=0; i < totalnets; i++) {
            total += HPWL[i];
        }
        return total;
    }

    bool checker(const  Cell* X, const Cell* Y, double temperature)
    {
        set<int> nets;
        if (X != NULL)
            for (int i = 0; i < X->nets.size(); i++)
            {
                nets.insert(X->nets[i]);
                // HPWL[X->nets[i]] = calculateHPWL(netlist[X->nets[i]]);
            }
        if (Y != NULL)
            for (int i = 0; i < Y->nets.size(); i++)
            {
                nets.insert(Y->nets[i]);
                // HPWL[Y->nets[i]] = calculateHPWL(netlist[Y->nets[i]]);
            }
        
        for (const auto& net : nets)
        {
            HPWL[net]=calculateHPWL(netlist[net]);
        }

        int total = calculateTotalHPWL();
        return total < temperature;
    }



    void annealing() {
        cout << "started annealing" << endl;
        double initialCost = calculateTotalHPWL();
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

                while (x_temp1 == x_temp2 && y_temp1 == y_temp2) {
                    x_temp1 = rand() % numRows;
                    x_temp2 = rand() % numRows;
                    y_temp1 = rand() % numColumns;
                    y_temp2 = rand() % numColumns;
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

                auto start = high_resolution_clock::now();
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
                auto stop = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(stop - start);
                cout << "Time taken by checker: " << duration.count() << " microseconds" << endl;
                
            }

            currentTemp *= 0.95;
        }

        cout << "Initial cost: " << initialCost << endl;
        cout << "Final Cost: " << calculateTotalHPWL() << endl;
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


int main() {
    placer place("t3.txt");
    place.run();
    return 0;
}