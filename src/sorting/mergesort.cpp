#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>

#include "mergesort.hpp"

using namespace std;

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    // Number of partitions to fit one partition in memSize memory.
    const uint k = size * sizeof(uint64_t) / memSize + 1;

    if (k < 2) {
        // All data fits into memory
        vector<uint64_t> partition(size);

        // Read from input file
        if (read(fdInput, &partition[0], 1) < 1) {
            cout << "No data found.\n";
            return;
        }

        // Sort
        sort(partition.begin(), partition.end());

        // Write to output file
        if (posix_fallocate(fdOutput, 0, size * sizeof(uint64_t)) != 0) {
            cout << "Could not allocate space for output file.\n";
        } else {
            if (write(fdOutput, &partition[0], size * sizeof(uint64_t)) < 0) {
                cout << "Error writing to output file.\n";
            }
        }
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
        char *inputFileName  = argv[1];
        char *outputFileName = argv[2];
        char *memSizeS       = argv[3];
        int  memSize         = stoi(memSizeS) * (0x1 << 20); // MB, thus * 2^20

        // Open input file
        int fdInput, fdOutput;
        if ((fdInput = open(inputFileName, O_RDONLY)) < 0) {
            cout << "Could not open input file. Aborting.\n";
            return EXIT_FAILURE;
        }

        if ((fdOutput = open(outputFileName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR|S_IWUSR)) < 0) {
            cout << "Could not open output file. Aborting.\n";
            return EXIT_FAILURE;
        }
        // Sort input to ouput
        externalSort(fdInput, 15, fdOutput, memSize);

        // Read output file
        // Validate output file

        close(fdInput);
        close(fdOutput);
    }
    return EXIT_SUCCESS;
}
