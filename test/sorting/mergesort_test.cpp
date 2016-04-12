#include "gtest/gtest.h"

TEST (MergeSortTest, EmptyStaysEmpty) {
}


TEST (MergeSortTest, SortedStaysSorted) {
}


TEST (MergeSortTest, ProperSorting) {
}


TEST (MergeSortTest, HandlesDuplicates) {
}


TEST (MergeSortTest, HandlesInvalidInputFileDescriptor) {
}


TEST (MergeSortTest, HandlesInvalidOutputFileDescriptor) {
}


TEST (MergeSortTest, HandlesInsufficientMemory) {
}


TEST (MergeSortTest, HandlesTooHighMemoryDemand) {
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
