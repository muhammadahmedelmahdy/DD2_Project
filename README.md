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
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

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

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/placer-project.git
   ```

2. Compile the code:

   ```bash
   g++ -o placer placer.cpp
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

## Project Structure

The project is organized as follows:

- `placer.cpp`: Main source code file containing the implementation.
- `input_netlist.txt`: Example input netlist file.

## Performance and Optimization

The code has passed through many optimization stages to reduce its time complexity and reducing the runtime from 8 hours to 10 minutes for the largest netlist file (t3.txt) This has been accomplished by using pointers. More methods to improve the runtime are to be implemented and here is what we are considering:
- Considering dividing the iterations and running them in parallel.
- Finding a method to optimize HPWL calculation.
- Reducing the number of available iterations.

## Testing

Comprehensive testing has been performed with all provided test files.

## Results and Analysis

