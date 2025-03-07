# Sudoku Solver+

Optimized C++ solver for Jane Street’s **Somewhat Square Sudoku Puzzle** (January 2025)  
_Maximize the GCD of the 9-digit row numbers while satisfying classic Sudoku constraints_

---

## Overview

**Sudoku Solver+** is an optimized solver written in C++ for Jane Street’s challenging **Somewhat Square Sudoku Puzzle**.  
This puzzle requires filling a 9x9 grid so that every row, column, and 3x3 sub-grid contains nine unique digits (selected from 0-9) while **maximizing the GCD** of the nine 9-digit numbers formed by each row. The solver leverages advanced algorithmic techniques, including multithreading, recursive backtracking, and bit-level optimizations to efficiently navigate the immense search space.

---

## Puzzle Information

- **Puzzle Name:** Jane Street “Somewhat Square Sudoku Puzzle”
- **Puzzle Date:** January 2025
- **Puzzle Link:** [Jane Street Somewhat Square Sudoku Puzzle](https://www.janestreet.com/puzzles/somewhat-square-sudoku-index/)


![image](https://github.com/user-attachments/assets/cb4a2245-81c6-4dca-9034-f07fb0e4dd6f)

---

## Key Features

- **Permutation Generation:**  
  Multithreaded generation of valid 9-digit candidate strings (omitting one digit at a time) to ensure only permissible combinations are considered.

- **Optimized Filtering:**  
  Rapid candidate elimination using modular arithmetic and bit masking to quickly filter out numbers that do not contain the required digits or meet divisibility conditions.

- **Parallel Processing:**  
  Utilizes multithreading and asynchronous tasks to distribute the workload, significantly enhancing performance.

- **Recursive Backtracking:**  
  Implements a recursive backtracking approach with optimized bitwise conflict-checking to efficiently traverse possible solutions.

- **Real-time Progress Monitoring:**  
  Periodic progress updates allow users to track the solving process, providing detailed insights into candidate attempts and runtime.

---

## Technologies Used

- **Programming Language:** Modern C++ (C++11 and above)
- **Parallel Computing:** Multithreading and asynchronous execution
- **Algorithmic Optimizations:**  
  - Bit masking  
  - Modular arithmetic  
  - Recursive backtracking

---

## Usage

Compile and run the solver using any standard C++ compiler. For example, with **g++**:

```bash
g++ -std=c++17 -O3 SudokuBox.cpp -o SudokuSolver+
./SudokuSolver+
Note: The solver has an approximate runtime of ~40 hours. Further optimization may be necessary depending on your system.
