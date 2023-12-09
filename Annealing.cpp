#include<vector>
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
   vector < int > nets;
};

class placer {
   public: placer(string filename) {
      parseInput(filename, totalcomponents);
      totalHPWL = 0;
      auto start = high_resolution_clock::now();
      initialPlacement(totalcomponents);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast < microseconds > (stop - start);
      printFinalPlacement();
      cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
      calculateInitialHPWL();

   }

   void run() {
      auto start = high_resolution_clock::now();
      annealing();
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast < microseconds > (stop - start);
      cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
      printFinalPlacement();
   }

   private: int numRows,
   numColumns;
   int totalcomponents,
   totalnets;
   vector < Cell * > components;
   vector < int > HPWL_X,
   HPWL_Y;
   vector < vector < Cell * >> netlist;
   vector < int > HPWL_Y_I,
   HPWL_X_I,
   HPWL_Y_I2,
   HPWL_X_I2;
   vector < vector < Cell * >> netlist_y;
   vector < vector < int >> grid;
   int totalHPWL;

   void parseInput(const string filename, int & totalcomponents) {
      ifstream netfile(filename);
      if (!netfile.is_open()) {
         cout << "File is not open" << endl;
         exit(0);
      }
      netfile >> totalcomponents >> totalnets >> numRows >> numColumns;
      grid.resize(numRows);

      for (int i = 0; i < totalcomponents; i++) {
         Cell * temp = new Cell;
         temp -> number = i;
         temp -> x = -1;
         temp -> y = -1;
         components.push_back(temp);
      }

      for (int i = 0; i < grid.size(); i++) {
         grid[i].resize(numColumns, -1);
      }

      netlist.reserve(totalnets);
      netlist_y.reserve(totalnets);
      HPWL_Y_I.reserve(totalnets);
      HPWL_X_I.reserve(totalnets);
      HPWL_Y_I2.reserve(totalnets);
      HPWL_X_I2.reserve(totalnets);
      for (int i = 0; i < totalnets; ++i) {
         int numberofcomponents;
         netfile >> numberofcomponents;
         vector < Cell * > net;
         net.reserve(numberofcomponents);
         for (int j = 0; j < numberofcomponents; j++) {
            int num;
            netfile >> num;
            components[num] -> nets.push_back(i);
            Cell * temp = components[num];
            net.push_back(temp);
         }
         netlist.push_back(net);
         netlist_y.push_back(net);
      }
      netfile.close();
   }

   void initialPlacement(int numTotalComponents) {
      vector < int > cellFills(numTotalComponents);
      generate(cellFills.begin(), cellFills.end(), [n = 0]() mutable {
         return n++;
      });
      cellFills.resize(numRows * numColumns);

      for (int i = numTotalComponents; i < numRows * numColumns; i++) {
         cellFills[i] = -1;
      }

      auto rd = random_device {};
      auto rng = default_random_engine {
         rd()
      };
      shuffle(begin(cellFills), end(cellFills), rng);

      int x = 0;
      for (int i = 0; i < numRows; i++) {
         for (int j = 0; j < numColumns; j++) {
            grid[i][j] = cellFills[x];
            if (cellFills[x] != -1 && cellFills[x] < numTotalComponents) {
               components[cellFills[x]] -> x = i;
               components[cellFills[x]] -> y = j;
            }
            x++;
         }
      }
   }

   static bool compare_X_coordinate(Cell * A, Cell * B) {
      return A -> x < B -> x;
   }

   static bool compare_Y_coordinate(Cell * A, Cell * B) {
      return A -> y < B -> y;
   }

   int calculateHPWL_X(vector < Cell * > & X) {
      sort(X.begin(), X.end(), compare_X_coordinate);
      int dx = X.back() -> x - X.front() -> x;
      return dx;
   }
   int calculateHPWL_Y(vector < Cell * > & Y) {
      sort(Y.begin(), Y.end(), compare_Y_coordinate);
      int dy = Y.back() -> y - Y.front() -> y;
      return dy;
   }

   void calculateInitialHPWL() {
      for (int i = 0; i < netlist.size(); i++) {
         int temp1 = calculateHPWL_X(netlist[i]);
         int temp2 = calculateHPWL_Y(netlist_y[i]);
         HPWL_X.push_back(temp1);
         HPWL_Y.push_back(temp2);
         HPWL_X_I.push_back(temp1);
         HPWL_Y_I.push_back(temp2);
         HPWL_X_I2.push_back(temp1);
         HPWL_Y_I2.push_back(temp2);
      }

      for (int i = 0; i < totalnets; i++) {
         totalHPWL += HPWL_X[i] + HPWL_Y[i];
      }
   }

   void checkHPWL_X(const Cell * cell, int net, int c) {
      if (cell == netlist[net].front()) {
            totalHPWL -= HPWL_X[net];
            HPWL_X[net] = calculateHPWL_X(netlist[net]);
            totalHPWL += HPWL_X[net];

      } else if (cell == netlist[net].back()) {
            totalHPWL -= HPWL_X[net];
            HPWL_X[net] = calculateHPWL_X(netlist[net]);
            totalHPWL += HPWL_X[net];
      } else {
         if (cell -> x > netlist[net].back() -> x || cell -> x < netlist[net].front() -> x) {
            totalHPWL -= HPWL_X[net];
            HPWL_X[net] = calculateHPWL_X(netlist[net]);
            totalHPWL += HPWL_X[net];
         } else {
            return;
         }
      }
   }

   void checkHPWL_Y(const Cell * cell, int net, int c) {
      if (cell == netlist_y[net].front()) {
            totalHPWL -= HPWL_Y[net];
            HPWL_Y[net] = calculateHPWL_Y(netlist_y[net]);
            totalHPWL += HPWL_Y[net];

      } else if (cell == netlist_y[net].back()) {
            totalHPWL -= HPWL_Y[net];
            HPWL_Y[net] = calculateHPWL_Y(netlist_y[net]);
            totalHPWL += HPWL_Y[net];
      } else {
         if (cell -> y > netlist_y[net].back() -> y || cell -> y < netlist_y[net].front() -> y) {
            totalHPWL -= HPWL_Y[net];
            HPWL_Y[net] = calculateHPWL_Y(netlist_y[net]);
            totalHPWL += HPWL_Y[net];
         } else {
            return;
         }
      }
   }

   bool checker(const int IHPWL1,
      const Cell * X,
         const Cell * Y, double temperature) {
      if (X != NULL) {
         for (int i = 0; i < X -> nets.size(); i++) {
            HPWL_X_I[X -> nets[i]] = HPWL_X[X -> nets[i]];
            HPWL_Y_I[X -> nets[i]] = HPWL_Y[X -> nets[i]];
         }
      }
      if (Y != NULL) {
         for (int i = 0; i < Y -> nets.size(); i++) {
            HPWL_X_I2[Y -> nets[i]] = HPWL_X[Y -> nets[i]];
            HPWL_Y_I2[Y -> nets[i]] = HPWL_Y[Y -> nets[i]];
         }
      }
      if (X != NULL) {
         for (int i = 0; i < X -> nets.size(); i++) {
            checkHPWL_X(X, X -> nets[i], 0);
            checkHPWL_Y(X, X -> nets[i], 0);
         }
      }

      if (Y != NULL) {
         for (int i = 0; i < Y -> nets.size(); i++) {
            checkHPWL_X(Y, Y -> nets[i], 1);
            checkHPWL_Y(Y, Y -> nets[i], 1);
         }
      }
      int deltaCost = IHPWL1 - totalHPWL;
      double random_number = static_cast < double > (std::rand()) / RAND_MAX;
      double e = 1 - exp(static_cast < double > (deltaCost) / temperature);
      bool check = ((deltaCost < 0 || deltaCost == 0) && (random_number) < e);
 /*     cout << deltaCost << "  " << temperature << "  " << random_number << "  " << e << "  " << check << "\t";
     for(int i = 0; i < HPWL_X.size(); i++){
     	cout << HPWL_X[i] << "  " << HPWL_Y[i] << "\t";
     }
     cout << "\n";*/
      return check;
   }

   void annealing() {
      cout << "started annealing" << endl;
      double initialCost = totalHPWL;
      double initialTemp = initialCost * 500;
      double finalTemp = 5 * pow(10, -6) * (initialCost / totalnets);
      double currentTemp = initialCost * 500;
      int IHPWL = initialCost;
      while (currentTemp > finalTemp) {
         int x_temp1, x_temp2, y_temp1, y_temp2;
         for (int i = 0; i < 10 * numRows * numColumns; ++i) {
            x_temp1 = rand() % numRows;
            x_temp2 = rand() % numRows;
            y_temp1 = rand() % numColumns;
            y_temp2 = rand() % numColumns;

            Cell * X = NULL;
            Cell * Y = NULL;

            int comp1 = grid[x_temp1][y_temp1];
            int comp2 = grid[x_temp2][y_temp2];

            if (comp1 != -1) {
               components[comp1] -> x = x_temp2;
               components[comp1] -> y = y_temp2;
               X = components[comp1];
            }
            if (comp2 != -1) {
               components[comp2] -> x = x_temp1;
               components[comp2] -> y = y_temp1;
               Y = components[comp2];
            }
            IHPWL = totalHPWL;
            bool checkk = checker(IHPWL, X, Y, currentTemp);
            //cout << checkk << "  " << currentTemp <<"\n";
            if (checkk == 0) {
               grid[x_temp1][y_temp1] = comp2;
               grid[x_temp2][y_temp2] = comp1;
            } else {
               if (comp1 != -1) {
                  components[comp1] -> x = x_temp1;
                  components[comp1] -> y = y_temp1;
                  for (int i = 0; i < components[comp1] -> nets.size(); i++) {
                     HPWL_X[components[comp1] -> nets[i]] = HPWL_X_I[components[comp1] -> nets[i]];
                     HPWL_Y[components[comp1] -> nets[i]] = HPWL_Y_I[components[comp1] -> nets[i]];
                  }
               }
               if (comp2 != -1) {
                  components[comp2] -> x = x_temp2;
                  components[comp2] -> y = y_temp2;
                  for (int i = 0; i < components[comp2] -> nets.size(); i++) {
                     HPWL_X[components[comp2] -> nets[i]] = HPWL_X_I2[components[comp2] -> nets[i]];
                     HPWL_Y[components[comp2] -> nets[i]] = HPWL_Y_I2[components[comp2] -> nets[i]];
                  }
               }
               totalHPWL = IHPWL;

            }
         }
         currentTemp *= 0.95;
      }

      cout << "Initial cost: " << initialCost << endl;
      cout << "Final Cost: " << totalHPWL << endl;
   }

   void printFinalPlacement() const {
      for (const auto & row: grid) {
         for (int cell: row) {
            if (cell == -1) {
               cout << "-" << "\t";
            } else {
               cout << setw(4) << setfill('0') << cell << "\t";
            }
         }
         cout << "\n";
      }

   }
};

void emitError(string s) {
   cout << s;
   exit(0);
}

int main(int argc, char *argv[]) {

   if (argc < 2) emitError("use: placer <netlist_file_name>\n");
   string filename = "Test Cases/" + string(argv[1]);
   cout << filename << endl;
   placer place(filename);
   place.run();
   return 0;
}
 