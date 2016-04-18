#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#include "mergesort.hpp"

using namespace std;

void externalSort(int fdInput, uint64_t size, int fdOutput, uint64_t memSize) {
    // Number of partitions to fit one partition in memSize memory.
    const uint k = size * sizeof(uint64_t) / memSize + 1;

    if (k < 2) {
        // All data fits into memory
        vector<uint64_t> partition(size);

        // Read from input file
        if (read(fdInput, &partition[0], size * sizeof(uint64_t)) < 1) {
            cout << "No data found." << endl;
            return;
        }

        // Sort
        sort(partition.begin(), partition.end());

        // Write to output file
        if (posix_fallocate(fdOutput, 0, size * sizeof(uint64_t)) != 0) {
            cout << "Could not allocate space for output file." << endl;
        } else {
            if (write(fdOutput, &partition[0], size * sizeof(uint64_t)) < 0) {
                cout << "Error writing to output file." << endl;
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
                cout << "Could not open output file. Aborting." << endl;
                return;
            }
            if (posix_fallocate(tmpFd, 0, partitionSize) != 0) {
               cout << "Could not allocate file space for partition." << endl;
                return;
            }
            if (write(tmpFd, &buffer[0], partitionSize) < 0) {
                cout << "Error writing partition." << endl;
                return;
            }
            lseek(tmpFd, 0, SEEK_SET);
            tmpFds[partitionNumber] = tmpFd;
        }

        if (partitionNumber < 0) {
            cout << "No data found." << endl;
            return;
        }

        // Merge step
        vector<vector<uint64_t>> segments(partitionNumber + 1);

        // Number of numbers per segment
        uint segmentSize = 5;


        // Load the initial batch of numbers
        for (int j = 0; j <= partitionNumber; j++) {
            vector<uint64_t> segment(segmentSize);
            if (read(tmpFds[j], &segment[0], segmentSize * sizeof(uint64_t)) > 0) {
                // TODO resize if not enough data was read
                reverse(segment.begin(), segment.end());
                segments[j] = segment;
            } else {
                cout << "Error reading from partition " << j << endl;
                for (int f = 0; f <= partitionNumber; f++) {
                    close(tmpFds[f]);
                }
                return;
            }
        }

        // Make space for output
        if (posix_fallocate(fdOutput, 0, size * sizeof(uint64_t)) != 0) {
            cout << "Could not allocate space for output file." << endl;
            for (int f = 0; f <= partitionNumber; f++) {
                close(tmpFds[f]);
            }
            return;
        }

        // Merge by dumping smallest number into file and reloading data if necessary
        while (segments.size() > 0) {
            uint minIndex = 0;
            uint64_t minElement = segments[0].back();

            // Find smallest element
            for (uint i = 0; i < segments.size(); i++) {
                if (segments[i].back() < minElement) {
                    minIndex = i;
                    minElement = segments[i].back();
                }
            }

            // Write smallest element to output file
            if (write(fdOutput, &minElement, sizeof(uint64_t)) < 0) {
                cout << "Error writing to output file" << endl;
                for (int f = 0; f <= partitionNumber; f++) {
                    close(tmpFds[f]);
                }
                return;
            }

            // Remove smallest element from its segment
            segments[minIndex].pop_back();
            if (segments[minIndex].size() < 1) {
                // Try loading new data
                vector<uint64_t> segment(segmentSize);
                if (read(tmpFds[minIndex], &segment[0], segmentSize * sizeof(uint64_t)) > 0) {
                    // TODO resize if not enough data was read
                    reverse(segment.begin(), segment.end());
                    segments[minIndex] = segment;
                } else {
                    // Else remove segment from list of segments
                    close(tmpFds[minIndex]);
                    segments.erase(segments.begin() + minIndex);
                }
            }
        }

        // Clean up after yourself
        for (int i = 0; i <= partitionNumber; i++) {
            sprintf(filename, ".tmpPart%i", i);
            if (remove(filename) != 0) {
                cout << "Error deleting tmp file." << endl;
            }
        }
    }
}

int validateOutputFile(int fdOutput) {
    lseek(fdOutput, 0, SEEK_SET);

    uint64_t oldElement = 0;
    uint64_t nextElement;

    cout << "Validating" << endl;
    while (read(fdOutput, &nextElement, sizeof(uint64_t)) > 0) {
        if (nextElement < oldElement) return -1;
        oldElement = nextElement;
    }
    return 0;
}


int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage: ./main <input file> <output file> <memory in MB>" << endl;
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

        if ((fdOutput = open(outputFileName, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR|S_IWUSR)) < 0) {
            cout << "Could not open output file. Aborting." << endl;
            close(fdInput);
            return EXIT_FAILURE;
        }

        struct stat stats;
        fstat(fdInput, &stats);
        // Sort input to ouput
        externalSort(fdInput, stats.st_size / sizeof(uint64_t), fdOutput, memSize);

        // Validate output file
        lseek(fdOutput, 0, SEEK_SET);
        if (validateOutputFile(fdOutput) != 0) {
            cout << "Validation failed." << endl;
            close(fdInput);
            close(fdOutput);
            return EXIT_FAILURE;
        }
        cout << "Validation successful" << endl;
        close(fdInput);
        close(fdOutput);
    }
    return EXIT_SUCCESS;
}
