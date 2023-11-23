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

vector<vector<Cell>> netlist;
vector<vector<int>> grid;

void  ParseInput(string FileName, int& numTotalComponents, int& numRows, int& numColumns) { //Function to store the netlist file in vectors.
    ifstream InetList(FileName);

    if (!InetList.is_open()) {
        cerr << "Unable to open the file." << endl;
        return;
    }

    int numNets;
    InetList >> numTotalComponents >> numNets >> numRows >> numColumns;
    grid.resize(numRows);

    for (int i = 0; i < grid.size(); i++)
        grid[i].resize(numColumns, -1);

    for (int i = 0; i < numNets; ++i) {
        int numComponents;
        InetList >> numComponents;

        vector<Cell> net;
        for (int j = 0; j < numComponents; ++j) {
            Cell cell;
            InetList >> cell.number;
            cell.x = -1; cell.y = -1;
            net.push_back(cell);
        }

        netlist.push_back(net);
    }

    InetList.close();
}

void swap(Cell& a, Cell& b) { //function to swap two cells together in case we are placing a cell in place of other cell.
    int temp1 = a.x;
    int temp2 = a.y;
    a.x = b.x;
    a.y = b.y;
    b.x = temp1;
    b.y = temp2;
}

bool compareX(Cell A, Cell B) {
    return A.x < B.x;
}

bool compareY(Cell A, Cell B) {
    return A.y < B.y;
}

int calculateHPWL(vector<vector<Cell>> Netlist)
{
    int total = 0;
    for (int i = 0; i < Netlist.size(); i++) {
        sort(Netlist[i].begin(), Netlist[i].end(), compareX);
        int dx = Netlist[i][Netlist[i].size() - 1].x - Netlist[i][0].x;
        sort(Netlist[i].begin(), Netlist[i].end(), compareY);
        int dy = Netlist[i][Netlist[i].size() - 1].y - Netlist[i][0].y;
        total += dx + dy;
    }
    return total;
}

bool checker(vector<vector<Cell>> tempNetlist, double temperature, double& currentTemp) { //function to assist in accepting or rejecting a move.

    int total = calculateHPWL(tempNetlist);

    if (total < temperature) {
        currentTemp = total;
        return true;
    }
    else {
        return false;
    }
}

void annealing(int numTotalComponents, int  numRows,int numColumns)
{    
    vector<int> cellFills(numTotalComponents);
    generate(cellFills.begin(), cellFills.end(), [n = 1] () mutable { return n++; });
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

    // wire length calculation using HPWL
    // double initialCost = calculateHPWL(netlist);
    // double initialTemp = initialCost * 500;
    // double finalTemp = 5*pow(10, -6)* initialCost;
    // double currentTemp = initialCost;

    // // while loop
    // vector<vector<Cell>> temp_netlist = netlist;

    // while (currentTemp > finalTemp)
    // {
    //     int x_temp1 = rand() % numRows;
    //     int x_temp2 = rand() % numRows;
    //     int y_temp1 = rand() % numColumns;
    //     int y_temp2 = rand() % numColumns;

    //     while (x_temp1 == x_temp2 && y_temp1 == y_temp2)
    //     {
    //         x_temp1 = rand() % numRows;
    //         x_temp2 = rand() % numRows;
    //         y_temp1 = rand() % numColumns;
    //         y_temp2 = rand() % numColumns;
    //     }

    //     int comp1 = grid[x_temp1][y_temp1];
    //     int comp2 = grid[x_temp2][y_temp2];

    //     for (int k = 0; k < temp_netlist.size(); k++)
    //     {
    //         for (int j = 0; j < temp_netlist[k].size(); j++)
    //         {
    //             if (temp_netlist[k][j].number == comp1)
    //             {
    //                 temp_netlist[k][j].x = x_temp2;
    //                 temp_netlist[k][j].y = y_temp2;

    //             }
    //             if (temp_netlist[k][j].number == comp2)
    //             {
    //                 temp_netlist[k][j].x = x_temp1;
    //                 temp_netlist[k][j].y = y_temp1;

    //             }                
    //         }
    //     }
    //     double nextTemp = currentTemp * 0.95;
    //     if (checker(temp_netlist, nextTemp, currentTemp))
    //     {
    //         grid[x_temp1][y_temp1] = grid[x_temp2][y_temp2];
    //         netlist = temp_netlist;
    //     }
    // }
}

void PrintFinalPlacment()
{
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            if (grid[i][j] == -1)
            {
                cout << "----" <<"\t";
            }
            else
            {
                cout << setw(4) << setfill('0') << grid[i][j] << "\t";
            }
        }
        cout << endl;
    }

}

void Run ()
{
    int numTotalComponents, numRows, numColumns;

    ParseInput("t3.txt", numTotalComponents, numRows, numColumns);
    cout << "done" << endl;
    auto start = high_resolution_clock::now();
    annealing(numTotalComponents, numRows, numColumns);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
    PrintFinalPlacment();

}

int main() {
    Run();
    return 0;
}
