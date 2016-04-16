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
                cout << "Error writing partition.";
                return;
            }
            
            tmpFds.push_back(tmpFd);
            close(tmpFd);
        }
        
        if (partitionNumber < 0) {
            cout << "No data found." << endl;
            return;    
        }
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

int validateOutputFile(int fdOutput){
	
	uint64_t oldElement = 0;
	uint64_t nextElement;

	while (read(fdOutput, &nextElement, sizeof(uint64_t)) > 0) {
    		if (nextElement < oldElement) return EXIT_FAILURE;
		//cout << nextElement << " größer als " << oldElement << endl;
    		oldElement = nextElement;
	}	

	cout << "Validation successful" << endl;

	return EXIT_SUCCESS;
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
	validateOutputFile(fdOutput);   

        close(fdInput);
        close(fdOutput);
    }
    return EXIT_SUCCESS;
}
