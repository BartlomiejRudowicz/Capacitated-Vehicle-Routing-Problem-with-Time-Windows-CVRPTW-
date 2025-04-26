# Capacitated Vehicle Routing Problem with Time Windows (CVRPTW)

This repository contains C++ implementations for solving the Capacitated Vehicle Routing Problem with Time Windows (CVRPTW), an NP-hard combinatorial optimization challenge. Two primary methodologies are implemented:

- **Iterated Local Search (ILS)**
- **Parameterized Randomized Algorithm**

## Project Overview

The CVRPTW requires determining optimal routes for a fleet of vehicles to deliver goods to a set of customers. Each vehicle has limited capacity, and deliveries must be made within specific time windows. The main goals are:
- Feasibility of the solution
- Minimizing the number of trucks
- Minimizing the total travel cost

## Algorithms Implemented

### 1. Iterated Local Search (ILS)
ILS algorithm refines an initial feasible solution by iteratively applying local search techniques. It incorporates stochastic processes to avoid local optima:
- Randomized removal of selected routes
- Rebuilding solutions using greedy heuristics based on proximity and feasibility
- Parameterized tuning to balance exploration and exploitation

### 2. Parameterized Randomized Algorithm
A customized randomized algorithm:
- Initially sorts customers based on closing time of their delivery windows
- Creates subsets of customers, mixing larger randomized sets with systematically selected smaller sets
- Parameter tuning optimizes the percentage split to improve overall route efficiency

## Features
- **Data Handling**: Reads Solomon-formatted data files (`spraw1000.txt`).
- **Flexibility**: Easy adjustments of parameters including truck capacity, number of trucks, and algorithm-specific settings.
- **Output Generation**: Solutions outputted to `wynik.txt` with clear formatting, including the number of trucks, total cost, and detailed routes.

## Files Included
- `ILS.cpp` – Implementation of Iterated Local Search
- `Sparametryzowany_losowy.cpp` – Implementation of Parameterized Randomized Algorithm
- PDF Reports detailing methodologies and parameter tuning:
  - `Sprawozdanie_ILS_CVRPTW.pdf`
  - `Sparametryzowany_algorytm_losowy_CVRPTW.pdf`

## Installation and Usage
### Compilation
Compile using g++:
```bash
g++ -o ils_solution ILS.cpp
./ils_solution

# or

g++ -o randomized_solution Sparametryzowany_losowy.cpp
./randomized_solution
```

### Running the Algorithms
Ensure the input data (`spraw1000.txt`) is placed in the executable's directory. Upon completion, results are saved in `wynik.txt`.

## Parameter Tuning
Extensive parameter tuning experiments have been conducted, results of which are available in the provided PDF reports. Recommended parameters are included directly within the source code.

## Complexity
Detailed computational complexity analysis is provided in the accompanying reports:
- Sorting and initial solution generation complexity
- Iterative improvement complexities for both algorithms


---

Developed by Igor Simon & Bartłomiej Rudowicz

