#include "Fakes/FakeBusDriver.hpp"
#include "RegisterAccess.hpp"

#include "gtest/gtest.h"

class RegisterAccessTest : public testing::Test
{
  protected:
    std::shared_ptr<FakeBusDriver> busDriver =
        std::make_shared<FakeBusDriver>();
    std::unique_ptr<RegisterAccess> accessor =
        std::make_unique<RegisterAccess>(busDriver);

    void SetUp() override
    {
        busDriver->clearMem();
    }
};

TEST_F(RegisterAccessTest, GetsUniqueRegisterContent)
{
    struct MyCustomRegister
    {
        char cstring[4];
    } __attribute__((aligned(1)));

    constexpr uint16_t offset = 0;

    MyCustomRegister* stubMemory =
        reinterpret_cast<MyCustomRegister*>(&busDriver->fakedMemory.at(offset));

    std::memcpy(&stubMemory->cstring, "123", 4);

    auto retrieved = accessor->get<MyCustomRegister>(offset);

    EXPECT_STREQ("123", retrieved.cstring);
}

TEST_F(RegisterAccessTest, SetsUniqueRegisterContent)
{
    struct MyCustomRegister
    {
        char cstring[4];
    } __attribute__((aligned(1)));

    constexpr uint16_t offset = 0;

    MyCustomRegister reg{.cstring = "987"};

    accessor->set(reg, offset);

    MyCustomRegister* stubMemory =
        reinterpret_cast<MyCustomRegister*>(&busDriver->fakedMemory.at(offset));

    EXPECT_STREQ(stubMemory->cstring, "987");
}
