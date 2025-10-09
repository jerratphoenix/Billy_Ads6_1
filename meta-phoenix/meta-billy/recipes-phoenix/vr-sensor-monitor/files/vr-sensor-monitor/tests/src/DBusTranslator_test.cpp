#include "DBusTranslator.hpp"

#include <functional>

#include "gtest/gtest.h"

using Func = std::function<void(void)>;

TEST(DBusTranslator, DoesNotThrowIfFunctorDoesNotThrow)
{
    Func foo = []() { return; };

    DBusTranslator translator(std::move(foo));

    EXPECT_NO_THROW({ translator(); });
}

TEST(DBusTranslator, ConvertsInvalidArgToEinval)
{
    Func foo = []() { throw std::invalid_argument("Invalid arg"); };

    DBusTranslator translator(std::move(foo));

    EXPECT_THROW(
        {
            try
            {
                translator();
            }
            catch (const sdbusplus::exception::SdBusError& e)
            {
                EXPECT_EQ(e.get_errno(), EINVAL);
                throw;
            }
            catch (...)
            {
                FAIL();
            }
        },
        sdbusplus::exception::SdBusError);
}

TEST(DBusTranslator, ConvertsRangeErrorToEinval)
{
    Func foo = []() { throw std::range_error("Range error"); };

    DBusTranslator translator(std::move(foo));

    EXPECT_THROW(
        {
            try
            {
                translator();
            }
            catch (const sdbusplus::exception::SdBusError& e)
            {
                EXPECT_EQ(e.get_errno(), EINVAL);
                throw;
            }
            catch (...)
            {
                FAIL();
            }
        },
        sdbusplus::exception::SdBusError);
}

TEST(DBusTranslator, RethrowsCompatibleType)
{
    Func foo = []() {
        throw sdbusplus::exception::SdBusError(EBUSY, "D-Bus exception");
    };

    DBusTranslator translator(std::move(foo));

    EXPECT_THROW(
        {
            try
            {
                translator();
            }
            catch (const sdbusplus::exception::SdBusError& e)
            {
                EXPECT_EQ(e.get_errno(), EBUSY);
                throw;
            }
            catch (...)
            {
                FAIL();
            }
        },
        sdbusplus::exception::SdBusError);
}

TEST(DBusTranslator, ConvertsStdExceptionToEio)
{
    Func foo = []() { throw std::exception(); };

    DBusTranslator translator(std::move(foo));

    EXPECT_THROW(
        {
            try
            {
                translator();
            }
            catch (const sdbusplus::exception::SdBusError& e)
            {
                EXPECT_EQ(e.get_errno(), EIO);
                throw;
            }
            catch (...)
            {
                FAIL();
            }
        },
        sdbusplus::exception::SdBusError);
}

TEST(DBusTranslator, ConvertsArbitraryExceptionToEio)
{
    Func foo = []() { throw 1; };

    DBusTranslator translator(std::move(foo));

    EXPECT_THROW(
        {
            try
            {
                translator();
            }
            catch (const sdbusplus::exception::SdBusError& e)
            {
                EXPECT_EQ(e.get_errno(), EIO);
                throw;
            }
            catch (...)
            {
                FAIL();
            }
        },
        sdbusplus::exception::SdBusError);
}
