#include <bits/stdc++.h>

using namespace std;

struct Cell {
    int number;
    int x;
    int y;
};

void ParseInput(string FileName){ //Function to store the netlist file in vectors.
        ifstream InetList(FileName);

    if (!InetList.is_open()) {
        cerr << "Unable to open the file." << endl;
        return;
    }

    int numTotalComponents, numNets, numRows, numColumns;
    InetList >> numTotalComponents >> numNets >> numRows >> numColumns;

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
}

void swap(Cell &a, Cell &b) { //function to swap two cells together in case we are placing a cell in place of other cell.
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
    for(int i = 0; i < netlist.size(); i++){
        sort(netlist[i].begin(), netlist[i].end(), compareX);
        int dx = netlist[i][netlist[i].size()-1].x - netlist[i][0].x;        
        sort(netlist[i].begin(), netlist[i].end(), compareY);
        int dy = netlist[i][netlist[i].size()-1].y - netlist[i][0].y;
        total += dx + dy;
    }

    if(total < temperature){
        return true;
    }
    else{
        return false;
    }
}

int main() {



    return 0;
}
