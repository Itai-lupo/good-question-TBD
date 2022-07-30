#include "log.hpp"

int main()
{
    logger::init("./logs/", "TBD");
    LOG_INFO("works");
    logger::close();
    return 0;
}