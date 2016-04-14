#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

#include "mergesort.hpp"

using namespace std;

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    /*
    - It is desirable to load the input file block wise to reduce seek time
    - memSize memory available
    - needs to contain both the buffer read from input file and the current partition
    - each number from input file is at least 2 bytes (1 numeral, 1 newline)
    - each number from input file is at max  21 bytes ("18446744073709551615" + 1 newline)
    */

    // Number of partitions to fit one partition in memSize memory.
    uint k = size * sizeof(uint64_t) / memSize;

    if (k < 2) {
        // All data fits into memory

        // Read from input file
            // Read block wise into buffer
            // Read lines from buffer
            // Parse line

        // Close input file

        // Sort

        // Write to output file

        // Close output file
    } else {
        // Read from input file
            // When as many elements as fit into a partition have been loaded
            // Sort partition
            // Allocate new file
            // Write partition to file
            // Store fd for tmp file

        // Close input file

        // Merge tmp files into output file
            // For each tmp file load first number
            // Find smallest number and write it to ouput file
            // Load next number from the file that contained the smallest number
            // Repeat until all tmp files are exhausted

        // Close output file

        // Remove tmp files
    }
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Insufficient arguments. Aborting." << endl;
        return EXIT_FAILURE;
    } else {
        // Get command line arguments
        char *inputFile  = argv[1];
        char *outputFile = argv[2];
        char *memSizeS   = argv[3];
        int  memSize     = stoi(memSizeS);

        cout << inputFile   << "\n" <<
                outputFile  << "\n" <<
                memSize     << "\n";

        // Open input file

        // Sort input to ouput

        // Read output file blockwise into buffer
            // Validate buffer contents
                // Read line
                // Parse line
                // Compare to previous line
                    // if less fault
                    // else continue
    }
    return EXIT_SUCCESS;
}
