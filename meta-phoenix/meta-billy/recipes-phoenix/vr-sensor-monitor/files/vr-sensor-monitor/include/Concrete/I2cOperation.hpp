#include <vector>
#include <cstdint>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <utility>
#include <string>

int i2cWriteRead(std::string i2cBus, const uint8_t slaveAddr,
        std::vector<uint8_t>& writeData, std::vector<uint8_t>& readBuf);

void i2cWriteReadWrap(std::string bus, const uint8_t addr,std::vector<uint8_t> wbuf, std::vector<uint8_t>& rbuf, size_t size /*= 0*/);

void i2cWriteWrap(std::string bus, const uint8_t addr,std::vector<uint8_t> wbuf);