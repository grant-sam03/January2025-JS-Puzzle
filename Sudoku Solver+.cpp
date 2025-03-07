#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <array>
#include <functional>
#include <chrono>
#include <unordered_set>
#include <thread>
#include <mutex>
using namespace std;

//--------------------------------------------------------------------
// This program solves the Jane Street "Somewhat Square Sudoku" puzzle (January 2025)
// https://www.janestreet.com/puzzles/somewhat-square-sudoku-index/
//
// The puzzle requires filling a 9x9 grid where:
// 1. Each row, column, and 3x3 box contains the same set of nine unique digits (using nine of the ten digits 0-9)
// 2. The GCD of the nine 9-digit numbers formed by the rows should be maximized
// 3. Some cells are already filled in as clues
//
// The answer to the puzzle is the 9-digit number formed by the middle row in the completed grid.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
// Helper functions (from your code)
//--------------------------------------------------------------------

// 1) Check if a number (9-char string) contains specific required digits
bool containsRequiredDigits(const string& number, const vector<char>& requiredDigits) {
    // Use a bit mask for faster checking
    int digitMask = 0;
    for (char c : number) {
        digitMask |= (1 << (c - '0'));
    }
    
    for (char digit : requiredDigits) {
        if ((digitMask & (1 << (digit - '0'))) == 0) {
            return false; // Missing a required digit
        }
    }
    return true;
}

// 2) Given a list of candidate strings, filter and keep only those divisible by candidateGCD.
vector<string> filterDivisibleByCandidate(const vector<string>& options, int candidateGCD) {
    vector<string> filtered;
    filtered.reserve(options.size() / 2); // Estimate capacity to avoid reallocations
    
    // Use a faster method for checking divisibility
    // For large numbers, we can use modular arithmetic instead of full conversion
    for (const string& opt : options) {
        // For 9-digit numbers, we can use a more efficient approach
        // than converting the entire string to a long long
        
        // Calculate remainder using modular arithmetic
        int remainder = 0;
        for (char c : opt) {
            remainder = (remainder * 10 + (c - '0')) % candidateGCD;
        }
        
        if (remainder == 0) {
            filtered.push_back(opt);
        }
    }
    return filtered;
}

// Optimized version of filterByColumn using reserve for better performance
vector<string> filterByColumn(const vector<string>& numbers, int column, char value) {
    vector<string> filtered;
    filtered.reserve(numbers.size() / 9); // Estimate capacity to avoid reallocations
    for (const string& number : numbers) {
        if (number[column] == value) {
            filtered.push_back(number);
        }
    }
    return filtered;
}

// Optimized version of filterDisallowedValues using reserve and a set for faster lookups
vector<string> filterDisallowedValues(const vector<string>& numbers,
                                      int column,
                                      const vector<char>& disallowedValues)
{
    // Convert disallowedValues to a set for O(1) lookups
    unordered_set<char> disallowedSet(disallowedValues.begin(), disallowedValues.end());
    
    vector<string> filtered;
    filtered.reserve(numbers.size()); // Reserve space to avoid reallocations
    
    for (const string& number : numbers) {
        if (disallowedSet.find(number[column]) == disallowedSet.end()) {
            filtered.push_back(number);
        }
    }
    return filtered;
}

//--------------------------------------------------------------------
// Main
//--------------------------------------------------------------------
int main() {
    // STEP 1. Generate all valid 9-digit strings with one digit missing
    vector<string> validNumbers;
    vector<char> requiredDigits = {'0','2','5'};
    
    // Start timing for performance measurement
    auto startGenTime = chrono::steady_clock::now();
    
    // Create a mutex for thread-safe vector access
    mutex validNumbersMutex;
    
    // Determine number of threads to use (leave one core free)
    unsigned int numThreads = max(1u, thread::hardware_concurrency() - 1);
    cout << "Using " << numThreads << " threads for permutation generation." << endl;
    
    // Vector to hold our threads
    vector<thread> threads;
    
    // Try skipping each digit (0-9) one at a time
    for (char skipDigit = '0'; skipDigit <= '9'; skipDigit++) {
        // Skip this iteration if the digit we want to skip is a required digit
        if (find(requiredDigits.begin(), requiredDigits.end(), skipDigit) != requiredDigits.end()) {
            cout << "Skipping digit '" << skipDigit << "' is not allowed as it's a required digit." << endl;
            continue;
        }
        
        // Create a string with all digits except the one we're skipping
        string digits;
        for (char d = '0'; d <= '9'; d++) {
            if (d != skipDigit) {
                digits.push_back(d);
            }
        }
        
        // Sort to prepare for permutation
        sort(digits.begin(), digits.end());
        
        // Create a thread to process this digit's permutations
        threads.emplace_back([digits, requiredDigits, skipDigit, &validNumbersMutex, &validNumbers]() {
            vector<string> localValidNumbers;
            string localDigits = digits;
            
            // Generate all permutations that contain the required digits
            size_t count = 0;
            do {
                if (containsRequiredDigits(localDigits, requiredDigits)) {
                    localValidNumbers.push_back(localDigits);
                }
                count++;
            } while (next_permutation(localDigits.begin(), localDigits.end()));
            
            // Now merge the local results with the global results
            {
                lock_guard<mutex> guard(validNumbersMutex);
                validNumbers.insert(validNumbers.end(), localValidNumbers.begin(), localValidNumbers.end());
                cout << "Skipping digit '" << skipDigit << "' generated " 
                     << localValidNumbers.size() << " valid strings from " << count << " permutations." << endl;
            }
        });
        
        // If we've reached our thread limit or this is the last digit, wait for threads to complete
        if (threads.size() >= numThreads || skipDigit == '9') {
            for (auto& t : threads) {
                if (t.joinable()) {
                    t.join();
                }
            }
            threads.clear();
        }
    }
    
    // Make sure all threads are joined
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    auto endGenTime = chrono::steady_clock::now();
    auto genDuration = chrono::duration_cast<chrono::milliseconds>(endGenTime - startGenTime).count();
    cout << "Generated " << validNumbers.size() << " valid 9-digit strings in " 
         << genDuration << " ms." << endl;
    
    // STEP 2. Build the base puzzle (row candidate lists) using your filtering functions.
    // (Positions use 0-indexing.)
    vector<vector<string>> puzzle(9);
    
    // Row1: fixed clue: column8 (index 7) must be '2'
    {
        vector<string> row1Options = filterByColumn(validNumbers, 7, '2');
        row1Options = filterDisallowedValues(row1Options, 2, {'0'});
        row1Options = filterDisallowedValues(row1Options, 4, {'0'});
        row1Options = filterDisallowedValues(row1Options, 6, {'5'});
        row1Options = filterDisallowedValues(row1Options, 8, {'5'});
        puzzle[0] = row1Options;
    }
    // Row2: fixed clues: column5 (index 4) is '2' and column9 (index 8) is '5'
    {
        vector<string> row2Options = filterByColumn(validNumbers, 4, '2');
        row2Options = filterByColumn(row2Options, 8, '5');
        row2Options = filterDisallowedValues(row2Options, 2, {'0'});
        row2Options = filterDisallowedValues(row2Options, 4, {'0'});
        puzzle[1] = row2Options;
    }
    // Row3: fixed clue: column2 (index 1) is '2'
    {
        vector<string> row3Options = filterByColumn(validNumbers, 1, '2');
        row3Options = filterDisallowedValues(row3Options, 2, {'0'});
        row3Options = filterDisallowedValues(row3Options, 4, {'0'});
        row3Options = filterDisallowedValues(row3Options, 6, {'5'});
        row3Options = filterDisallowedValues(row3Options, 7, {'5'});
        row3Options = filterDisallowedValues(row3Options, 8, {'5'});
        puzzle[2] = row3Options;
    }
    // Row4: fixed clue: column3 (index 2) is '0'
    {
        vector<string> row4Options = filterByColumn(validNumbers, 2, '0');
        row4Options = filterDisallowedValues(row4Options, 1, {'2'});
        row4Options = filterDisallowedValues(row4Options, 3, {'2'});
        row4Options = filterDisallowedValues(row4Options, 4, {'2'});
        row4Options = filterDisallowedValues(row4Options, 5, {'2'});
        row4Options = filterDisallowedValues(row4Options, 7, {'2'});
        row4Options = filterDisallowedValues(row4Options, 6, {'5'});
        row4Options = filterDisallowedValues(row4Options, 8, {'5'});
        puzzle[3] = row4Options;
    }
    // Row5: no fixed digit, but some disallowed columns
    {
        vector<string> row5Options = validNumbers;
        row5Options = filterDisallowedValues(row5Options, 0, {'0'});
        row5Options = filterDisallowedValues(row5Options, 1, {'0','2'});
        row5Options = filterDisallowedValues(row5Options, 2, {'0'});
        row5Options = filterDisallowedValues(row5Options, 4, {'0','2'});
        row5Options = filterDisallowedValues(row5Options, 6, {'5'});
        row5Options = filterDisallowedValues(row5Options, 8, {'5'});
        puzzle[4] = row5Options;
    }
    // Row6: fixed clue: column4 (index 3) is '2'
    {
        vector<string> row6Options = filterByColumn(validNumbers, 3, '2');
        row6Options = filterDisallowedValues(row6Options, 0, {'0'});
        row6Options = filterDisallowedValues(row6Options, 1, {'0'});
        row6Options = filterDisallowedValues(row6Options, 2, {'0'});
        row6Options = filterDisallowedValues(row6Options, 4, {'0'});
        row6Options = filterDisallowedValues(row6Options, 6, {'5'});
        row6Options = filterDisallowedValues(row6Options, 8, {'5'});
        puzzle[5] = row6Options;
    }
    // Row7: fixed clue: column5 (index 4) is '0'
    {
        vector<string> row7Options = filterByColumn(validNumbers, 4, '0');
        row7Options = filterDisallowedValues(row7Options, 1, {'2'});
        row7Options = filterDisallowedValues(row7Options, 3, {'2'});
        row7Options = filterDisallowedValues(row7Options, 5, {'2'});
        row7Options = filterDisallowedValues(row7Options, 7, {'2'});
        row7Options = filterDisallowedValues(row7Options, 6, {'5'});
        row7Options = filterDisallowedValues(row7Options, 7, {'5'});
        row7Options = filterDisallowedValues(row7Options, 8, {'5'});
        puzzle[6] = row7Options;
    }
    // Row8: fixed clue: column6 (index 5) is '2'
    {
        vector<string> row8Options = filterByColumn(validNumbers, 5, '2');
        row8Options = filterDisallowedValues(row8Options, 2, {'0'});
        row8Options = filterDisallowedValues(row8Options, 3, {'0'});
        row8Options = filterDisallowedValues(row8Options, 4, {'0'});
        row8Options = filterDisallowedValues(row8Options, 6, {'5'});
        row8Options = filterDisallowedValues(row8Options, 7, {'5'});
        row8Options = filterDisallowedValues(row8Options, 8, {'5'});
        puzzle[7] = row8Options;
    }
    // Row9: fixed clue: column7 (index 6) is '5'
    {
        vector<string> row9Options = filterByColumn(validNumbers, 6, '5');
        row9Options = filterDisallowedValues(row9Options, 1, {'2'});
        row9Options = filterDisallowedValues(row9Options, 3, {'2'});
        row9Options = filterDisallowedValues(row9Options, 4, {'2'});
        row9Options = filterDisallowedValues(row9Options, 5, {'2'});
        row9Options = filterDisallowedValues(row9Options, 7, {'2'});
        row9Options = filterDisallowedValues(row9Options, 2, {'0'});
        row9Options = filterDisallowedValues(row9Options, 3, {'0'});
        row9Options = filterDisallowedValues(row9Options, 4, {'0'});
        row9Options = filterDisallowedValues(row9Options, 5, {'0'});
        puzzle[8] = row9Options;
    }
    
    // Print the number of candidate options per row from the base puzzle.
    for (int r = 0; r < 9; r++) {
        cout << "Row " << r+1 << " has " << puzzle[r].size() << " candidate(s) (before GCD filtering)." << endl;
    }
    
    // STEP 3. Convert candidate strings to vectors of digits.
    // We'll keep the base candidates (before the candidate-GCD filtering) in "puzzle".
    // (We will re-filter for each candidate GCD below.)
    // For convenience, we save the base puzzle in "basePuzzle" (as a copy).
    vector<vector<string>> basePuzzle = puzzle;
    
    // Optimize GCD candidate search - for Jane Street puzzle, we want to maximize the GCD
    // We'll cycle over candidate GCD values from highest to lowest for efficiency
    // Only try those that end in 1, 3, 7, or 9 (as these are coprime to 10)
    vector<int> candidateGCDs;
    for (int candidateGCD = 12345678; candidateGCD >= 337; candidateGCD--) {
        int lastDigit = candidateGCD % 10;
        if (lastDigit == 1 || lastDigit == 3 || lastDigit == 7 || lastDigit == 9) {
            candidateGCDs.push_back(candidateGCD);
        }
    }
    
    cout << "Testing " << candidateGCDs.size() << " candidate GCDs in descending order." << endl;
    
    for (int candidateGCD : candidateGCDs) {
        // Create a new puzzle filtered by candidateGCD.
        vector<vector<string>> candidatePuzzle = basePuzzle;
        bool validCandidate = true;
        for (int r = 0; r < 9; r++) {
            candidatePuzzle[r] = filterDivisibleByCandidate(candidatePuzzle[r], candidateGCD);
            if (candidatePuzzle[r].empty()) {
                validCandidate = false;
                break;
            }
        }
        if (!validCandidate) continue;
        
        // For each row, convert candidate strings to vectors of digits.
        vector<vector<vector<int>>> candidates(9);
        for (int r = 0; r < 9; r++) {
            for (const string &s : candidatePuzzle[r]) {
                vector<int> cand;
                for (char c : s) {
                    cand.push_back(c - '0');
                }
                candidates[r].push_back(cand);
            }
        }
        
        // We'll use a fixed ordering based on our candidate counts.
        vector<int> rowOrder = { 1, 8, 5, 3, 6, 7, 2, 0, 4 }; // 0-indexed row indices
        
        // Initialize constraint masks and solution grid.
        array<int, 9> colMask = {0,0,0,0,0,0,0,0,0};
        array<int, 9> boxMask = {0,0,0,0,0,0,0,0,0};
        vector<vector<int>> solution(9, vector<int>(9, 0));
        vector<vector<vector<int>>> allSolutions;
        
        unsigned long long candidateTries = 0;
        auto startTime = chrono::steady_clock::now();
        auto lastProgressUpdate = startTime;
        const int PROGRESS_UPDATE_INTERVAL = 30; // seconds (changed from 15 to 30)
        
        // Set up a separate thread for progress reporting
        bool solverRunning = true;
        cout << "Starting solver for GCD " << candidateGCD << "..." << endl;
        
        auto progressThread = thread([&]() {
            while (solverRunning) {
                this_thread::sleep_for(chrono::seconds(PROGRESS_UPDATE_INTERVAL));
                auto currentTime = chrono::steady_clock::now();
                auto totalElapsed = chrono::duration_cast<chrono::seconds>(currentTime - startTime).count();
                
                cout << "Progress update - GCD: " << candidateGCD 
                     << ", Candidates tried: " << candidateTries 
                     << ", Total time: " << totalElapsed << "s" << endl;
                cout.flush(); // Force output to display
            }
        });
        
        // Helper lambda: check if at least one of the first columns has a 0
        auto hasZeroInFirstColumns = [&solution]() -> bool {
            // Check for any 0 in first three columns
            for (int c = 0; c < 3; c++) {
                for (int r = 0; r < 9; r++) {
                    if (solution[r][c] == 0) {
                        return true;
                    }
                }
            }
            return false;
        };
        
        // Helper lambda: return the 3x3 box index for cell (r, c)
        auto getBoxIndex = [](int r, int c) -> int {
            return (r / 3) * 3 + (c / 3);
        };
        
        // Precompute box indices for each cell to avoid repeated calculations
        array<array<int, 9>, 9> boxIndices;
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                boxIndices[r][c] = getBoxIndex(r, c);
            }
        }
        
        // Recursive backtracking using the fixed ordering.
        function<void(int)> solveFixed = [&](int pos) {
            // Remove the timing-based progress reporting from here
            
            if (pos == 9) {
                // Verify we have at least one 0 in the first columns before accepting the solution
                if (hasZeroInFirstColumns()) {
                    allSolutions.push_back(solution);
                }
                return;
            }
            
            int r = rowOrder[pos];
            
            for (const auto &cand : candidates[r]) {
                candidateTries++;
                
                // Fast conflict check using bitmasks
                bool conflict = false;
                
                // Unrolled loop optimization for first few checks which are most likely to fail
                if (true) {  // Always execute first check
                    int d = cand[0];
                    int b = boxIndices[r][0];
                    if ((colMask[0] & (1 << d)) || (boxMask[b] & (1 << d))) {
                        conflict = true;
                    }
                }
                
                if (!conflict) {  // Only check second column if first didn't conflict
                    int d = cand[1];
                    int b = boxIndices[r][1];
                    if ((colMask[1] & (1 << d)) || (boxMask[b] & (1 << d))) {
                        conflict = true;
                    }
                }
                
                // Check remaining columns if no conflict found
                if (!conflict) {
                    for (int c = 2; c < 9; c++) {
                        int d = cand[c];
                        int b = boxIndices[r][c];
                        if ((colMask[c] & (1 << d)) || (boxMask[b] & (1 << d))) {
                            conflict = true;
                            break;
                        }
                    }
                }
                
                if (conflict) continue;
                
                // Save old masks for backtracking
                array<int, 9> oldColMask = colMask;
                array<int, 9> oldBoxMask = boxMask;
                
                // Update masks and solution
                for (int c = 0; c < 9; c++) {
                    int d = cand[c];
                    colMask[c] |= (1 << d);
                    boxMask[boxIndices[r][c]] |= (1 << d);
                    solution[r][c] = d;
                }
                
                solveFixed(pos + 1);
                
                // Restore masks for backtracking
                colMask = oldColMask;
                boxMask = oldBoxMask;
            }
        };
        
        solveFixed(0);
        
        // Stop the progress reporting thread
        solverRunning = false;
        if (progressThread.joinable()) {
            progressThread.join();
        }
        
        if (!allSolutions.empty()) {
            cout << "\nFound solution with GCD " << candidateGCD << " (highest possible):" << endl;
            cout << "The puzzle has " << allSolutions.size() << " solution(s)." << endl;
            
            int solCount = 0;
            for (const auto &sol : allSolutions) {
                solCount++;
                cout << "\nSolution #" << solCount << ":" << endl;
                for (int r = 0; r < 9; r++) {
                    for (int c = 0; c < 9; c++) {
                        cout << sol[r][c];
                    }
                    cout << "\n";
                }
                
                // Print the answer (middle row) as required by the Jane Street puzzle
                cout << "\nJane Street Puzzle Answer (middle row): ";
                for (int c = 0; c < 9; c++) {
                    cout << sol[4][c];
                }
                cout << endl;
            }
            
            cout << "\nFor GCD " << candidateGCD 
                 << ", total candidate rows tried: " << candidateTries << endl;
            
            // Since we're searching from highest to lowest GCD, we can break after finding the first valid solution
            break;
        } else {
            if (candidateTries > 0) {
                cout << "Candidate GCD " << candidateGCD << " yields no solutions after trying " 
                     << candidateTries << " candidates." << endl;
            }
        }
    }
    
    return 0;
}
