Jane Street "Somewhat Square Sudoku Solver"

This repository contains an optimized solver for Jane Street's "Somewhat Square Sudoku Puzzle" from January 2025, written in C++. The puzzle involves filling a 9x9 Sudoku grid with digits such that each row, column, and 3x3 sub-grid contains nine unique digits (selected from 0-9), with the goal of maximizing the greatest common divisor (GCD) of the 9-digit numbers formed by each row.

Puzzle Information

Puzzle Link: Jane Street "Somewhat Square Sudoku Puzzle"

 ![image](https://github.com/user-attachments/assets/d9f201c5-1540-44af-9345-27298fc36dcd)


Key Features

Permutation Generation: Multithreaded generation of valid candidate numbers, optimized for speed and efficiency.

Optimized Filtering: Leveraged modular arithmetic and bit masking to rapidly eliminate invalid candidates.

Parallel Processing: Utilized multithreading and asynchronous tasks to efficiently manage computational workload and significantly enhance solver speed.

Recursive Backtracking: Implemented recursive backtracking with optimized bitwise conflict-checking to quickly find valid solutions.

Real-time Progress Monitoring: Included periodic progress updates, allowing users to track solving performance in real-time.

Technologies Used

Modern C++ (<thread>, <mutex>, <unordered_set>, <chrono>, <bitset>)

Parallel Computing and Asynchronous Execution

Algorithmic Optimizations (bit masking, modular arithmetic)

Puzzle Source

Jane Street Somewhat Square Sudoku Puzzle (January 2025)

Outcome

Efficiently solved the puzzle, successfully meeting the unique constraint of maximizing the GCD across all rows.

Demonstrated proficiency in multithreading, algorithmic optimization, and computational problem-solving.

Usage

Compile and run using any standard C++ compiler supporting C++11 or later:

g++ -std=c++17 -O3 SudokuBox.cpp -o SudokuSolver
./SudokuBox

Skills Highlighted

Algorithmic optimization

Multithreaded and concurrent programming

Computational efficiency and performance profiling

Please Note: This has ~40hr runtime :) (not sure how to optimize it anymore)

Complex logic implementation and debugging

Puzzle Reference

Jane Street's Somewhat Square Sudoku (January 2025)
