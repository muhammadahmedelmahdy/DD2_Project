#include <vector>
#include<iostream>
#include<string>
#include<fstream>
#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<cmath>
#include<unordered_map>
#include<iomanip>

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
    netlist.resize(numNets);
    grid.resize(numRows);

    for (int i = 0; i < grid.size(); i++)
        grid[i].resize(numColumns);

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

bool checker(vector<vector<Cell>> tempNetlist, double temperature, double& currentTemp) { //function to assist in accepting or rejecting a move.

    int total = 0;
    for (int i = 0; i < tempNetlist.size(); i++) {
        sort(tempNetlist[i].begin(), tempNetlist[i].end(), compareX);
        int dx = tempNetlist[i][tempNetlist[i].size() - 1].x - tempNetlist[i][0].x;
        sort(tempNetlist[i].begin(), tempNetlist[i].end(), compareY);
        int dy = tempNetlist[i][tempNetlist[i].size() - 1].y - tempNetlist[i][0].y;
        total += dx + dy;
    }

    if (total < temperature) {
        currentTemp = total;
        return true;
    }
    else {
        return false;
    }
}
int calculateHPWL(vector<vector<Cell>> netlist)
{
    int totalHPWL = 0;

    for (const auto& net : netlist) {
        if (net.empty()) {
            continue;
        }
        int minX = net[0].x;
        int minY = net[0].y;
        int maxX = net[0].x;
        int maxY = net[0].y;
        for (const auto& cell : net) {
            minX = min(minX, cell.x);
            minY = min(minY, cell.y);
            maxX = max(maxX, cell.x);
            maxY = max(maxY, cell.y);
        }
        totalHPWL += (maxX - minX) + (maxY - minY);
    }
    return totalHPWL;
}
void annealing(int numTotalComponents, int  numRows,int numColumns)
{
    unordered_map<int, int> placedx;
    unordered_map<int, int> placedy;
    //get the connections from the netlist
    // do random placement
    srand(time(0));
    for (int i = 0; i < numRows; i++)
    {
            for (int j = 0; j < numColumns; j++)
            {
                grid[i][j] = -1;

            }
    }

    for (int i = 1; i <= numTotalComponents; i++)
    {
        int x_temp = rand() % numRows;
        while (placedx.find(x_temp) != placedx.end())
        {
            x_temp = rand() % numRows;
        }
        placedx[x_temp] = i;
        int y_temp = rand() % numColumns;
        while (placedy.find(y_temp) != placedy.end())
        {
            y_temp = rand() % numColumns;
        }
        placedy[y_temp] = i;

        grid[x_temp][y_temp] = i;
        for (int k = 1; k < netlist.size(); k++)
        {
            for (int j = 1; j < netlist[k].size(); j++)
            {
                if (netlist[k][j].number == i)
                {
                    netlist[k][j].x = x_temp;
                    netlist[k][j].y = y_temp;

                }
            }
        }
    }
    cout << "done" << endl;
    // wire length calculation using HPWL
    double initialCost = calculateHPWL(netlist);
    double initialTemp = initialCost * 500;
    double finalTemp = 5*pow(10, -6)* initialCost;
    double currentTemp = initialCost;

    // while loop
    vector<vector<Cell>> temp_netlist = netlist;

    while (currentTemp > finalTemp)
    {
        int x_temp1 = rand() % numRows;
        int x_temp2 = rand() % numRows;
        int y_temp1 = rand() % numColumns;
        int y_temp2 = rand() % numColumns;

        while (x_temp1 == x_temp2 && y_temp1 == y_temp2)
        {
            x_temp1 = rand() % numRows;
            x_temp2 = rand() % numRows;
            y_temp1 = rand() % numColumns;
            y_temp2 = rand() % numColumns;
        }

        int comp1 = grid[x_temp1][y_temp1];
        int comp2 = grid[x_temp2][y_temp2];

        for (int k = 0; k < temp_netlist.size(); k++)
        {
            for (int j = 0; j < temp_netlist[k].size(); j++)
            {
                if (temp_netlist[k][j].number == comp1)
                {
                    temp_netlist[k][j].x = x_temp2;
                    temp_netlist[k][j].y = y_temp2;

                }
                if (temp_netlist[k][j].number == comp2)
                {
                    temp_netlist[k][j].x = x_temp1;
                    temp_netlist[k][j].y = y_temp1;

                }                
            }
        }
        double nextTemp = currentTemp * 0.95;
        if (checker(temp_netlist, nextTemp, currentTemp))
        {
            grid[x_temp1][y_temp1] = grid[x_temp2][y_temp2];
            netlist = temp_netlist;
        }
    }
}

void PrintFinalPlacment()
{
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            cout << setw(4) << setfill('0') << grid[i][j];
        }
    }
}

void Run ()
{
    int numTotalComponents, numRows, numColumns;

    ParseInput("d1.txt", numTotalComponents, numRows, numColumns);
    cout << "done" << endl;
    annealing(numTotalComponents, numRows, numColumns);
    cout << "done" << endl;
    PrintFinalPlacment();

}

int main() {
    Run();
    return 0;
}
