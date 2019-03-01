#include "test_AtomicQueue.hpp"
#include "test_ThreadPool.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}