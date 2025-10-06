#include "EfficientMap.hpp"

#include <array>

#include "gtest/gtest.h"

enum class MyTestEnum
{
    Field,
    AnotherField,
    FieldThatIsNotInTheMap
};

static constexpr auto testMap =
    EfficientMap(std::array<std::pair<const char*, MyTestEnum>, 2>{
        {{"Field", MyTestEnum::Field},
         {"AnotherField", MyTestEnum::AnotherField}}});

static constexpr auto invalidTestMap =
    EfficientMap(std::array<std::pair<const char*, MyTestEnum>, 2>{
        {{nullptr, MyTestEnum::Field}}});

TEST(EfficientMap, CorrectlyFindsEnumBasedOnString)
{
    {
        constexpr char stringToFind[]("Field");
        auto foundEnum = testMap.getValue(stringToFind);

        EXPECT_EQ(foundEnum, MyTestEnum::Field);
    }
    {
        constexpr char stringToFind[]("AnotherField");
        auto foundEnum = testMap.getValue(stringToFind);

        EXPECT_EQ(foundEnum, MyTestEnum::AnotherField);
    }
}

TEST(EfficientMap, ThrowsRangeErrorWhenCouldntFindStringInMap)
{
    constexpr char stringToFind[]("ThisFieldIsDefinitelyNotInTheMap");

    EXPECT_THROW({ testMap.getValue(stringToFind); }, std::range_error);
}

TEST(EfficientMap, CorrectlyFindsStringBasedOnEnum)
{
    {
        constexpr MyTestEnum enumToFind = MyTestEnum::Field;
        auto foundString = testMap.getKey(enumToFind);

        EXPECT_STREQ(foundString, "Field");
    }
    {
        constexpr MyTestEnum enumToFind = MyTestEnum::AnotherField;
        auto foundString = testMap.getKey(enumToFind);

        EXPECT_STREQ(foundString, "AnotherField");
    }
}

TEST(EfficientMap, ThrowsRangeErrorWhenCouldntFindEnumInMap)
{
    constexpr MyTestEnum enumToFind = MyTestEnum::FieldThatIsNotInTheMap;

    EXPECT_THROW({ testMap.getKey(enumToFind); }, std::range_error);
}

TEST(EfficientMap, ThrowsLogicErrorIfContainerIsMalformed)
{
    constexpr MyTestEnum enumToFind = MyTestEnum::Field;
    constexpr char stringToFind[]("ThisFieldIsDefinitelyNotInTheMap");

    EXPECT_THROW({ invalidTestMap.getKey(enumToFind); }, std::logic_error);

    EXPECT_THROW({ invalidTestMap.getValue(stringToFind); }, std::logic_error);
}
