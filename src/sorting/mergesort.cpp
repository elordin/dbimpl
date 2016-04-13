#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <queue>
#include <algorithm>

#include "mergesort.hpp"

using namespace std;

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {

    // Number of partitions to fit one partition in memSize memory.
    int k = size * sizeof(uint64_t) / memSize;

    // Max number of members of a single partition
    uint64_t partitionSize = size / k;

    int lineNum = 0;
    int pindex  = 0;

    char *line;

    vector<uint64_t> partitionBuffer = vector<uint64_t>();

    while (/* TODO Read next line from fdInput*/false) {

        int i = stoi(line);

        // Append i to partition
        partitionBuffer.push_back(i);

        lineNum++;
        if (lineNum % partitionSize == 0) {
            // TODO Create new tmp file for partition #pindex

            sort(partitionBuffer.begin(), partitionBuffer.end());

            // TODO Write sorted partitionBuffer to tmp file

            partitionBuffer.clear();
            pindex++;
        }
    }

    // TODO Merge tmp files into output file

}


#ifdef EXERCISE_1

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
        */
    }
    return EXIT_SUCCESS;

}

#endif
