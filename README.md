# Simulated Annealing-Based Placer

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [Running the Placer](#running-the-placer)
  - [Output](#output)
- [Project Structure](#project-structure)
- [Performance and Optimization](#performance-and-optimization)
- [Testing](#testing)
- [Results and Analysis](#results-and-analysis)

## Introduction

This repository contains an implementation of a simulated annealing-based placer designed to minimize the total wirelength in a given netlist. The placer follows the guidelines outlined in the project statement, including the definition of HPWL (half-perimeter wirelength) and the use of a cooling schedule for optimization.

## Features

- Random initial placement of components on a 2D grid.
- Simulated annealing algorithm to optimize the placement.
- Calculation of total wirelength based on HPWL for each net.
- Temperature-based acceptance criteria for placement changes.
- Performance measurement and optimization strategies.

## Getting Started

### Prerequisites

- C++ compiler (supporting C++14 or higher)
For Annimation purpose
- CImg-dev library
- ImageMagick

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/placer-project.git
   ```

2. Compile the code:

   ```bash
   g++ -o placer placer.cpp
   ```
3. For annimation:
   For Ubuntu:
   a. install CImg-dev library
    ```bash
    sudo apt-get install -y cimg-dev
    ```
   b. install ImageMagick
    ```bash
    sudo apt-get install imagemagick
    ```
   c. compile the code
    ```bash
    g++ -o placer placer.cpp -lX11
    ```
   

## Usage

### Running the Placer

Execute the compiled binary:

```bash
./placer input_netlist.txt
```

Replace `input_netlist.txt` with the path to your netlist file.

### Output

The placer will display the final placements, along with the total wirelength before and after the annealing process.

### Running the placer with annimation

1. Create a folder named images in the same directory
2. run the code
    ```bash
    ./place input_netlist.txt
    ```
3. in you directory go to images folder and run this command
    ```bash
    convert -resize 50% -delay 10 -loop 0 image_{0..[lastimagenumber]}.jpg output.gif
    ```
### Output

you will get a GIF that simulates the whole placing process with black representing full cells and white representing empty cells.
## Project Structure

The project is organized as follows:

- `placer.cpp`: Main source code file containing the implementation.
- `input_netlist.txt`: Example input netlist file.

## Performance and Optimization

The code has passed through many optimization stages to reduce its time complexity and reducing the runtime from 8 hours to around 20 seconds for the largest netlist file (t3.txt) This has been accomplished by using pointers. More methods to improve the runtime are to be implemented and here is what we are considering:
- Considering dividing the iterations and running them in parallel.
- Finding a method to optimize HPWL calculation.
- Reducing the number of available iterations.

## Testing

Comprehensive testing has been performed with all provided test files.

## Results and Analysis
![image](https://github.com/muhammadahmedelmahdy/DD2_Project/assets/57318849/889a1611-00f6-4a44-9643-4550960cabd5)
![image](https://github.com/muhammadahmedelmahdy/DD2_Project/assets/57318849/65c78c2e-da6b-40d2-b38d-73ce1461c8ae)
![image](https://github.com/muhammadahmedelmahdy/DD2_Project/assets/57318849/fdacd2b2-60d5-4074-a649-f60c9b9ed259)


