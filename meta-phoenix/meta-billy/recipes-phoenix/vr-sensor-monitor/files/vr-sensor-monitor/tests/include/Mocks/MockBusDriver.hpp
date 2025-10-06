#pragma once

#include "Interfaces/IBusDriver.hpp"

#include "gmock/gmock.h"

class MockBusDriver : public IBusDriver
{
  public:
    MOCK_METHOD(void, write,
                (const uint16_t offset, const std::vector<uint8_t>& buffer),
                (override));

    MOCK_METHOD(void, read,
                (const uint16_t offset, std::vector<uint8_t>& buffer),
                (override));
};
