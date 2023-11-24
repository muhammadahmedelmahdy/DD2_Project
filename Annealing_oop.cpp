#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include<string>
#include<unordered_map>

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
        make_initialPlacement(totalcomponents);

    }
    void run() {
        auto start = high_resolution_clock::now();
        do_annealing();
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        std::cout << "Time taken by function: " << duration.count() << " microseconds" << std::endl;
        // printFinalPlacement();
    }

private:
    int numRows, numColumns;
    int totalcomponents;
    std::vector<std::vector<Cell>> netlist;
    std::vector<std::vector<int>> grid;
    void parseInput(const string filename, int& totalcomponents)
    {
        ifstream netfile(filename);
        if (!netfile.is_open())
        {
            cout << "File is not open" << endl;
            return;
        }
        int totalnets;
        netfile >> totalcomponents >> totalnets >> numRows >> numColumns;
        grid.resize(numRows);
        for (int i = 0; i < grid.size(); i++)
        {
            grid[i].resize(numColumns, -1);
        }
        for (int i = 0; i < totalnets; ++i) {
            int numberofcomponents;
            netfile >> numberofcomponents;
            vector<Cell> net;
            for (int j = 0; j < numberofcomponents; j++)
            {
                Cell temp;
                netfile >> temp.number;
                temp.x = -1;
                temp.y = -1;
                net.push_back(temp);
            }
            netlist.push_back(net);
        }

        netfile.close();
    }

    void swap_cells(Cell& a, Cell& b) {
        std::swap(a.x, b.x); std::swap(a.y, b.y);
    }
    static bool compare_X_coordinate(Cell& A, Cell& B)
    {
        return A.x < B.x;
    }
    static bool compare_Y_coordinate(Cell& A, Cell& B)
    {
        return A.y < B.y;
    }
    int findNetHPWL(vector<Cell> net)
    {
        vector<Cell> X(net);
        vector<Cell> Y(net);
        sort(X.begin(), X.end(), compare_X_coordinate);
        sort(Y.begin(), Y.end(), compare_Y_coordinate);
        int dx = X.back().x - X.front().x;
        int dy = Y.back().y - Y.front().y;
        return dx + dy;
    }
    int find_total_HPWL(vector<vector<Cell>> Netlist) {
        int total = 0;
        for (auto net : Netlist) {
            total += findNetHPWL(net);
        }
        return total;
    }
    bool checker(vector<vector<Cell>>& tempNetlist, double temperature) {
        int total = find_total_HPWL(tempNetlist);
        return total < temperature;
    }


    void do_annealing() {
        cout << "started annealing" << endl;
        double initialCost = find_total_HPWL(netlist);
        double initialTemp = initialCost * 500;
        double finalTemp = 5 * pow(10, -6) * initialCost;
        double currentTemp = initialCost;

        std::vector<std::vector<Cell>> tempNetlist = netlist;

        std::srand(std::time(0));
        while (currentTemp > finalTemp) {
            for (int i = 0; i < 10 * numRows * numColumns; ++i) {
                int x_temp1 = std::rand() % numRows;
                int x_temp2 = std::rand() % numRows;
                int y_temp1 = std::rand() % numColumns;
                int y_temp2 = std::rand() % numColumns;

                while (x_temp1 == x_temp2 && y_temp1 == y_temp2) {
                    x_temp1 = std::rand() % numRows;
                    x_temp2 = std::rand() % numRows;
                    y_temp1 = std::rand() % numColumns;
                    y_temp2 = std::rand() % numColumns;
                }

                int comp1 = grid[x_temp1][y_temp1];
                int comp2 = grid[x_temp2][y_temp2];

                for (auto& net : tempNetlist) {
                    for (auto& cell : net) {
                        if (cell.number == comp1) {
                            cell.x = x_temp2;
                            cell.y = y_temp2;
                        }
                        if (cell.number == comp2) {
                            cell.x = x_temp1;
                            cell.y = y_temp1;
                        }
                    }
                }

                double nextTemp = currentTemp * 0.95;
                if (checker(tempNetlist, nextTemp)) {
                    grid[x_temp1][y_temp1] = comp2;
                    grid[x_temp2][y_temp2] = comp1;
                    netlist = tempNetlist;
                }
            }

            currentTemp *= 0.95;
        }

        std::cout << "Initial cost: " << initialCost << std::endl;
        std::cout << "Final Cost: " << find_total_HPWL(netlist) << std::endl;
    }
    void make_initialPlacement(int numTotalComponents)
    {
        vector<int> cellFills(numTotalComponents);
        generate(cellFills.begin(), cellFills.end(), [n = 1]() mutable { return n++; });
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
            for (int j = 0; j < numColumns; j++)
                grid[i][j] = cellFills[x++];

        for (int i = 0; i < netlist.size(); i++)
        {
            for (int j = 0; j < netlist[i].size(); j++)
            {
                auto it = find(cellFills.begin(), cellFills.end(), netlist[i][j].number);
                if (it != cellFills.end())
                {
                    int index = it - cellFills.begin();
                    netlist[i][j].x = index / numColumns;
                    netlist[i][j].y = index % numColumns;
                }

            }
        }

        cout << "initial placement done" << endl;

    }


    void printFinalPlacement() const {
        for (const auto& row : grid) {
            for (int cell : row) {
                if (cell == -1) {
                    std::cout << "----" << "\t";
                }
                else {
                    std::cout << '\t' << "0000" << cell << "\t";
                }
            }
            std::cout << std::endl;
        }
    }






};
int main() {
    placer place("C:/Users/Muhammed/Desktop/DD2_Project/d1.txt");
    place.run();
    return 0;
}