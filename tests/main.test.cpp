#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include "log.hpp"

int main(int argc, char **argv) {
    std::cout << argv[1] << std::endl; 
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}

TEST(log, log)
{
    logger::init("./logs/", "TBD");
    LOG_INFO("works");
    logger::close();
}