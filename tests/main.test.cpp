#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include "log.hpp"
#include "entityPool.hpp"
#include "componentType.hpp"

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

TEST(entityPool, create)
{
    entityPool test(10);
    componentType testType(&test, 4);
    componentType *testType2 = new componentType(&test, 4);

    entityId testEntitys[15];
    
    for(int i = 0; i < 15; i++)
    {
        testEntitys[i] = test.allocEntity();
        ASSERT_EQ(testEntitys[i].index, i) << " allocated index is worng";
        ASSERT_EQ(testEntitys[i].gen, 0) << " allocated gen is worng";
    }

    for(int i = 0; i < 15; i++)
    {
        testEntitys[i] = test.allocEntity();
        ASSERT_EQ(testEntitys[i].index, i + 15) << " allocated index is worng";
        ASSERT_EQ(testEntitys[i].gen, 0) << " allocated gen is worng";
    }

    for(int i = 0; i < 15; i++)
        test.allocEntity();


    for(int i = 0; i < 15; i++)
    {
        ASSERT_EQ(testType.getComponent(testEntitys[i]), nullptr) << "I didn't set this id data";
        testType.setComponent(testEntitys[i], new int(i));
        ASSERT_EQ(*(int*)testType.getComponent(testEntitys[i]), i) << "value was set to i";
        testType.setComponent(testEntitys[i], new int(i * i));
        ASSERT_EQ(*(int*)testType.getComponent(testEntitys[i]), i * i) << "value was set to i * i";

    }

    for(int i = 0; i < 15; i++)
        ASSERT_EQ(*(int*)testType.getComponent(testEntitys[i]), i * i) << "value was set to i * i";
    
    for(int i = 0; i < 15; i++)
    {
        testType.deleteComponent(testEntitys[i]);
        ASSERT_EQ(testType.getComponent(testEntitys[i]), nullptr) << "value was deleted";
        
        ASSERT_EQ(testType.getComponent(testEntitys[i]), nullptr) << "I didn't set this id data";
        testType.setComponent(testEntitys[i], new int(i));
        ASSERT_EQ(*(int*)testType.getComponent(testEntitys[i]), i) << "value was set to i";
        testType.setComponent(testEntitys[i], new int(i * i));
        ASSERT_EQ(*(int*)testType.getComponent(testEntitys[i]), i * i) << "value was set to i * i";
    }
    
    delete testType2;
    for(uint8_t j = 1, k = 0; k != 255; j++, k += (j % 2))
    {
        for(int i = 0; i < 15; i++)
        {
            test.freeEntity(testEntitys[i]);
            testEntitys[i].gen++;
            ASSERT_EQ(testType.getComponent(testEntitys[i]), nullptr) << "value was deleted";

        }

        for(int i = 0; i < 15; i++)
        {
            testEntitys[i] = test.allocEntity();
            ASSERT_EQ(testEntitys[i].index, i + 15) << " allocated index is worng";
            ASSERT_EQ(testEntitys[i].gen, j) << " allocated gen is worng";
        }
    }


    ASSERT_FALSE(test.isIdValid({46, 0})) << "45 should be the highset allocated id";
    ASSERT_FALSE(test.isIdValid({16, 0})) << "id 16 is deleted many times";
    ASSERT_FALSE(test.isIdValid({10, 1})) << "this should be of gen 0";
    ASSERT_FALSE(test.isIdValid({40, 1})) << "this should be of gen 0";

}