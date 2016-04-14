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

uint64_t loadNextInt(int fd) {
    string line;
    // TODO Read line from file
    return stoi(line);
}

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    // Number of partitions to fit one partition in memSize memory.
    uint k = size * sizeof(uint64_t) / memSize;

    vector<uint64_t> partitionBuffer = vector<uint64_t>();
    uint lineNum = 0;

    if (k < 2) {
        // All data fits into memory

        // Read from file
        while (lineNum < size) {
            int i = loadNextInt(fdInput);
            partitionBuffer.push_back(i);
            lineNum++;
        }

        sort(partitionBuffer.begin(), partitionBuffer.end());
        // TODO Write sorted partitionBuffer to output file
    } else {
        // Max number of members of a single partition
        uint64_t partitionSize = size / k;

        int pindex  = 0;

        vector<int> tmpFds = vector<int>();

        while (lineNum < size) {
            int i = loadNextInt(fdInput);
            partitionBuffer.push_back(i);
            lineNum++;

            // Dump sorted partition into file when buffer reaches partition-size
            if (lineNum % partitionSize == 0) {
                sort(partitionBuffer.begin(), partitionBuffer.end());

                // TODO Create new tmp file for partition #pindex
                // TODO Append fd to tmpFds
                // TODO Write sorted partitionBuffer to tmp file

                partitionBuffer.clear();
                pindex++;
            }
        }

        // TODO Merge tmp files into output file
    }
}

/*
int main(int argc, char **argv) {

    if (argc < 4) {

        cout << "Insufficient arguments. Aborting.\n";
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

        // open input file

        // sort input to ouput

        // read output file
        */
        /*
        // validate output file
        int prevValue = -1;
        int currValue = -1;
        int line = 0;
        while ((currValue = parse_line) > -1) {
            line++;
            if (currValue < prevValue) {
                cout << "Unsorted values found in line " << line << ". Aborting. \n";
                return EXIT_FAILURE;
            }
            prevValue = currValue;
        }
        *//*
    }
    return EXIT_SUCCESS;

}
*/
