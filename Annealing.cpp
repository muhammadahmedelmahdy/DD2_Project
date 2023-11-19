#include <vector>
#include<iostream>
#include<string>
#include<fstream>
#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<unordered_map>
using namespace std;

struct Cell {
    int number;
    int x;
    int y;
};

vector<vector<Cell>> ParseInput(string FileName, int& numCells,int & numNets,int& numRows,int& numColumns) { //Function to store the netlist file in vectors.
    ifstream InetList(FileName);

    if (!InetList.is_open()) {
        cerr << "Unable to open the file." << endl;
        return;
    }

   
    InetList >> numCells >> numNets >> numRows >> numColumns;

    vector<vector<Cell>> netlist;

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
    return netlist;
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

bool checker(vector<vector<Cell>> netlist, double temperature) { //function to assist in accepting or rejecting a move.

    int total = 0;
    for (int i = 0; i < netlist.size(); i++) {
        sort(netlist[i].begin(), netlist[i].end(), compareX);
        int dx = netlist[i][netlist[i].size() - 1].x - netlist[i][0].x;
        sort(netlist[i].begin(), netlist[i].end(), compareY);
        int dy = netlist[i][netlist[i].size() - 1].y - netlist[i][0].y;
        total += dx + dy;
    }

    if (total < temperature) {
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
void annealing(vector<vector<Cell>> netlist, int numCells,int  numRows,int numColumns)
{
    unordered_map<int, int> placedx;
    unordered_map<int, int> placedy;
    vector<vector<Cell>> grid(numRows);
    for (int i = 0; i < grid.size(); i++)
        grid[i].resize(numColumns);
    //get the connections from the netlist
    // do random placement
    srand(time(0));
    for (int i = 1; i <= numCells; i++)
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

        Cell temp;
        temp.number = i;
        temp.x = x_temp;
        temp.y = y_temp;
        grid[x_temp][y_temp] = temp;
        for (int k = 1; k < netlist.size(); k++)
        {
            for (int j = 1; j < netlist[i].size(); j++)
            {
                if (netlist[k][j].number == i)
                {
                    netlist[k][j].x = x_temp;
                    netlist[k][j].y = y_temp;

                }
            }
        }
    }
    // wire length calculation using HPWL
    int HPWL = calculateHPWL(netlist);
    // while loop



}
int main() {





    return 0;
}
