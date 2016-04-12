#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    // TODO
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
