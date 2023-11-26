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
#include <random>

using namespace std::chrono;
using namespace std;

struct Cell {
    int number;
    int x;
    int y;
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
    vector<vector<Cell*>> netlist;
    vector<vector<int>> grid;
    unordered_map<int, vector<int>> netToComp;

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
                netToComp[num].push_back(j);
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
        generate(cellFills.begin(), cellFills.end(), [n = 0]() mutable { return n++; });
        cellFills.resize(numRows * numColumns);

        for (int i = numTotalComponents; i < numRows * numColumns; i++)
        {
            cellFills[i] = -1;
        }

        std::random_device rd;
        auto rng = default_random_engine{ rd() };
        shuffle(begin(cellFills), end(cellFills), rng);

        int x = 0;
        for (int i = 0; i < numRows; i++)
        {
            for (int j = 0; j < numColumns; j++)
            {
                grid[i][j] = cellFills[x];
                if (cellFills[x] != -1 && cellFills[x] < numTotalComponents)
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


    int calculateTotalHPWL(vector<vector<Cell*>> Netlist, int comp, int comp2)
    {
        int total = 0;
        auto it1 = netToComp[comp].begin();
        auto it2 = netToComp[comp2].begin();
        while (it1 != netToComp[comp].end() && it2 != netToComp[comp2].end())
        {
            if (*it1 < *it2)
            {
                total += calculateHPWL(Netlist[*it1]);
                it1++;
            }
            else
            {
                if (*it2 < *it1)
                {
                    total += calculateHPWL(Netlist[*it2]);
                    it2++;

                }
                else
                {
                    total += calculateHPWL(Netlist[*it2]);
                    it2++;
                    it1++;

                }
            }
        }
        while (it1 != netToComp[comp].end())
        {

            total += calculateHPWL(Netlist[*it1]);
            it1++;
        }

        while (it2 != netToComp[comp2].end())
        {

            total += calculateHPWL(Netlist[*it2]);
            it2++;
        }


        return total;





    }

    int calculateTotalHPWLI(const vector<vector<Cell*>>& Netlist) {
        int total = 0;
        for (const auto& net : Netlist) {
            total += calculateHPWL(net);
        }
        return total;
    }


    bool checker(const vector<vector<Cell* >>& tempNetlist, double temperature, int comp, int comp2)
    {
        int total = calculateTotalHPWL(tempNetlist, comp, comp2);
        return total < temperature;
    }



    void annealing() {
        cout << "started annealing" << endl;
        double initialCost = calculateTotalHPWLI(netlist);
        double initialTemp = initialCost * 500;
        double finalTemp = 5 * pow(10, -6) * initialCost / totalnets;
        double currentTemp = initialTemp;

        srand(time(0));
        while (currentTemp > finalTemp) {
            for (int i = 0; i < 10 * numRows * numColumns; ++i) {
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

                int comp1 = grid[x_temp1][y_temp1];
                int comp2 = grid[x_temp2][y_temp2];
                if (comp1 != -1)
                {
                    components[comp1]->x = x_temp2;
                    components[comp1]->y = y_temp2;
                }
                if (comp2 != -1)
                {
                    components[comp2]->x = x_temp1;
                    components[comp2]->y = y_temp1;
                }

                if (checker(netlist, currentTemp, comp1, comp2)) {
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
        cout << "Final Cost: " << calculateTotalHPWLI(netlist) << endl;
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
    placer place("C:/Users/Muhammed/Desktop/DD2_Project/d1.txt");
    place.run();
    return 0;
}