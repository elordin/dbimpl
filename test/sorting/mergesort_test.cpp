#include "gtest/gtest.h"

TEST (MergeSortTest, EmptyStaysEmpty) {
    // e.g. sort([]) == []
}


TEST (MergeSortTest, SortedStaysSorted) {
    // e.g. sort([1,4,7,15,103]) = [1,4,7,15,103]
}


TEST (MergeSortTest, ProperSorting) {
    // e.g. sort([4,7,15,103,1]) = [1,4,7,15,103]
}


TEST (MergeSortTest, HandlesDuplicates) {
    // e.g. sort([4,7,15,103,7,1]) = [1,4,7,7,15,103]
    // e.g. sort([4,7,7,15,103,1]) = [1,4,7,7,15,103]
    // e.g. sort([7,7,7,7,7,7,7])  = [7,7,7,7,7,7,7]
}


TEST (MergeSortTest, HandlesInvalidInputFileDescriptor) {
}


TEST (MergeSortTest, HandlesInvalidOutputFileDescriptor) {
}


TEST (MergeSortTest, HandlesInsufficientMemory) {
}


TEST (MergeSortTest, HandlesTooHighMemoryDemand) {
}

