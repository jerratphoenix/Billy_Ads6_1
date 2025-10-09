#include "Concrete/I2cOperation.hpp"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include <utility>
#include <string>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


#include <cstdio>
#include <sys/types.h>

int i2cWriteRead(std::string i2cBus, const uint8_t slaveAddr,
        std::vector<uint8_t>& writeData, std::vector<uint8_t>& readBuf)
{

    /* the following code is a duplication of i2cWriteRead() on libipmid/utils.cpp */
    int i2cDev = ::open(i2cBus.c_str(), O_RDWR | O_CLOEXEC);
    if (i2cDev < 0)
    {
        return -2;
    }

    const size_t writeCount = writeData.size();
    const size_t readCount = readBuf.size();
    int msgCount = 0;
    i2c_msg i2cmsg[2] = {};
    if (writeCount)
    {
        // Data will be writtern to the slave address
        i2cmsg[msgCount].addr = slaveAddr;
        i2cmsg[msgCount].flags = 0x00;
        i2cmsg[msgCount].len = writeCount;
        i2cmsg[msgCount].buf = writeData.data();
        msgCount++;
    }
    if (readCount)
    {
        // Data will be read into the buffer from the slave address
        i2cmsg[msgCount].addr = slaveAddr;
        i2cmsg[msgCount].flags = I2C_M_RD;
        i2cmsg[msgCount].len = readCount;
        i2cmsg[msgCount].buf = readBuf.data();
        msgCount++;
    }

    i2c_rdwr_ioctl_data msgReadWrite = {};
    msgReadWrite.msgs = i2cmsg;
    msgReadWrite.nmsgs = msgCount;

    // Perform the combined write/read
    int ret = ::ioctl(i2cDev, I2C_RDWR, &msgReadWrite);
    ::close(i2cDev);

    if (ret < 0)
    {
        return -1;
    }
    if (readCount)
    {
        readBuf.resize(msgReadWrite.msgs[msgCount - 1].len);
    }

    return 0;
}
void i2cWriteReadWrap(std::string bus, const uint8_t addr,std::vector<uint8_t> wbuf, std::vector<uint8_t>& rbuf, size_t size = 0)
{
    rbuf.resize(size);
    std::string dev = "/dev/i2c-" + bus;
    i2cWriteRead("/dev/i2c-" + bus, addr, wbuf, rbuf);
}

void i2cWriteWrap(std::string bus, const uint8_t addr,std::vector<uint8_t> wbuf)
{
    std::vector<uint8_t> buf;
    i2cWriteReadWrap(bus,addr,std::move(wbuf), buf, 0);
}