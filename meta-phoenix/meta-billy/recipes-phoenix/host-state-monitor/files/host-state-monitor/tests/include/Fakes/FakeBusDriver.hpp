#pragma once

#include "Interfaces/IBusDriver.hpp"

#include <array>
#include <cstring>

class FakeBusDriver : public IBusDriver
{
  public:
    void write(const uint16_t offset,
               const std::vector<uint8_t>& buffer) override
    {
        std::memcpy(&fakedMemory.at(offset), buffer.data(), buffer.size());
    }

    void read(const uint16_t offset, std::vector<uint8_t>& buffer) override
    {
        std::memcpy(buffer.data(), &fakedMemory.at(offset), buffer.size());
    }

    void clearMem()
    {
        std::memset(fakedMemory.data(), 0, fakedMemory.size());
    }

    std::array<uint8_t, 256> fakedMemory{};
};
