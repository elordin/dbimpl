#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>

#include "mergesort.hpp"

using namespace std;

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    // Number of partitions to fit one partition in memSize memory.
    const uint k = 2; // size * sizeof(uint64_t) / memSize + 1;
    
    if (k < 2) {
        // All data fits into memory
        vector<uint64_t> partition(size);

        // Read from input file
        if (read(fdInput, &partition[0], size * sizeof(uint64_t)) < 1) {
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
        // uint partitionSize = memSize - (memSize % sizeof(uint64_t)); // Don't read partial numbers
        
        // Byte size of the partition
        uint partitionSize = size / k * sizeof(uint64_t);
        // List of all tmp files
        vector<int> tmpFds(k);
        vector<uint64_t> buffer(size / k);
        int tmpFd;
        int partitionNumber = -1;
        char filename[32];
        
        // Read from input file
        while (read(fdInput, &buffer[0], partitionSize) > 0) {
      
            // Sort
            sort(buffer.begin(), buffer.end());
                        
            partitionNumber++;
            
            // New filename
            sprintf(filename, ".tmpPart%i", partitionNumber);
            
            if ((tmpFd = open(filename, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR|S_IWUSR)) < 0) {
                cout << "Could not open output file. Aborting.\n";
                return;
            }
            if (posix_fallocate(tmpFd, 0, partitionSize) != 0) {
               cout << "Could not allocate file space for partition.\n";
                return;
            }            
            if (write(tmpFd, &buffer[0], partitionSize) < 0) {
                cout << "Error writing partition." << endl;
                return;
            }
            
            tmpFds.push_back(tmpFd);
        }
        
        cout << partitionNumber << endl;
        
        if (partitionNumber < 0) {
            cout << "No data found." << endl;
            return;    
        }
        
        /*
        uint entriesPerPartition = memSize / sizeof(uint64_t) / (partitionNumber + 1);
        cout << entriesPerPartition << endl;
        
        vector<vector<uint64_t>> numbers(partitionNumber + 1);
        
        cout << numbers.size() << endl;
        
        for (int i = 0; i <= partitionNumber; i++) {
            vector<uint64_t> partitionSegment(entriesPerPartition);
            
            if (read(tmpFds[i], &partitionSegment[0], entriesPerPartition * sizeof(uint64_t)) < 1) {
                cout << "Empty partition found." << endl;
            }
            
            reverse(partitionSegment.begin(), partitionSegment.end());
            numbers[i] = partitionSegment;
        }
        
        for (uint i = 0; i < numbers.size(); i++) {
            for (uint j = 0; j < numbers[i].size(); j++) {
                cout << numbers[i][j] << " | ";
            }
            cout << endl;
        }
        */
        
        /*
        while (numbers.size() > 1) {
            // Find smallest number
            
            int smallestIndex = 0; 
            for (uint i = 0; i <= numbers.size(); i++) {
                if (numbers[i].back() < numbers[smallestIndex].back()) {
                    smallestIndex = i;
                }
            }
            if (write(fdOutput, &(numbers[smallestIndex].back()), sizeof(uint64_t)) < 0) {
                cout << "Error writing result." << endl;
                return;
            }
            numbers[smallestIndex].pop_back();
            if (numbers[smallestIndex].size() < 1) {
                // Reload more data 
                // If none is available, close and remove from numbers
            }
        }
        */
        // Merge tmp files into output file
            // For each tmp file load first number
            // Find smallest number and write it to ouput file
            // Load next number from the file that contained the smallest number
            // Repeat until all tmp files are exhausted

        // Write last vector en bloc

        // Remove tmp files
    }
}

int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage: ./main <input file> <output file> <memory in MB>" << endl;
        return EXIT_SUCCESS;
    } else {
        // Get command line arguments
        char *inputFileName  = argv[1];
        char *outputFileName = argv[2];
        char *memSizeS       = argv[3];
        int  memSize         = stoi(memSizeS) * (0x1 << 20); // MB, thus * 2^20

        // Open input file
        int fdInput, fdOutput;
        if ((fdInput = open(inputFileName, O_RDONLY)) < 0) {
            cout << "Could not open input file. Aborting." << endl;
            return EXIT_FAILURE;
        }

        if ((fdOutput = open(outputFileName, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR|S_IWUSR)) < 0) {
            cout << "Could not open output file. Aborting." << endl;
            return EXIT_FAILURE;
        }
        // Sort input to ouput
        // TODO Where do we get the size parameter?
        externalSort(fdInput, 20, fdOutput, memSize);

        // Read output file
        // Validate output file

        close(fdInput);
        close(fdOutput);
    }
    return EXIT_SUCCESS;
}
