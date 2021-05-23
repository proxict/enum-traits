// Make sure we are running our tests with expected constrains
#undef ENUM_TRAITS_MAX_NAME_LENGTH // 64
#undef ENUM_TRAITS_MIN_ENUM_VALUE // std::numeric_limits<int8_t>::min()
#undef ENUM_TRAITS_MAX_ENUM_VALUE // std::numeric_limits<int8_t>::max()

#include "enum-traits/enum-traits.hpp"

#include <gmock/gmock.h>

using namespace etraits;

using ::testing::ElementsAre;

TEST(enumTraits, minMax) {
    enum class Fruits { Apples = 2, Oranges = 5, Pears = 8, Bananas = 11 };
    EXPECT_EQ(EnumTraits<Fruits>::min(), 2);
    EXPECT_EQ(EnumTraits<Fruits>::max(), 11);

    enum class Cities : int8_t { Prague = -128, Berlin = 127 };
    EXPECT_EQ(EnumTraits<Cities>::min(), -128);
    EXPECT_EQ(EnumTraits<Cities>::max(), 127);

    enum class Countries { Germany = -1000, Czechia = 1000 };
    EXPECT_EQ(EnumTraits<Countries>::min(), 127);
    EXPECT_EQ(EnumTraits<Countries>::max(), -128);

    enum Dogs : uint8_t { Bulldog, Poodle, Rottweiler, Husky = 255 };
    EXPECT_EQ(EnumTraits<Dogs>::min(), 0);
    EXPECT_EQ(EnumTraits<Dogs>::max(), 2); // Husky doesn't fit into the allowed maximum
                                           // of int8_t (127), next one is Rottweiler (2)

    enum class Cats : int64_t { British = -7, Persian = -6, Bengal = -5, Siberian = -4 };
    EXPECT_EQ(EnumTraits<Cats>::min(), -7);
    EXPECT_EQ(EnumTraits<Cats>::max(), -4);

    enum class Hamsters { MyHamster = 10, Stranger = -10 };
    EXPECT_EQ(EnumTraits<Hamsters>::min(), -10);
    EXPECT_EQ(EnumTraits<Hamsters>::max(), 10);

}

TEST(enumTraits, values) {
    {
        enum class Fruits { Apples = 2, Oranges = 5, Pears = 8, Bananas = 11 };
        constexpr auto values = EnumTraits<Fruits>::values();
        EXPECT_THAT(values, ElementsAre(Fruits::Apples, Fruits::Oranges, Fruits::Pears, Fruits::Bananas));
    }

    {
        enum class Cities { Berlin = -128, Prague = 127 };
        constexpr auto values = EnumTraits<Cities>::values();
        EXPECT_THAT(values, ElementsAre(Cities::Berlin, Cities::Prague));
    }

    {
        enum class Hamsters { MyHamster = 10, Stranger = -10 };
        constexpr auto values = EnumTraits<Hamsters>::values();
        EXPECT_THAT(values, ElementsAre(Hamsters::Stranger, Hamsters::MyHamster));
    }
}

TEST(enumTraits, names) {
    {
        enum class Fruits { Apples = 2, Oranges = 5, Pears = 8, Bananas = 11 };
        constexpr auto names = EnumTraits<Fruits>::names();
        EXPECT_THAT(names, ElementsAre("Apples", "Oranges", "Pears", "Bananas"));
    }

    {
        enum class Cities { Prague = -127, Berlin = 126 };
        constexpr auto names = EnumTraits<Cities>::names();
        EXPECT_THAT(names, ElementsAre("Prague", "Berlin"));
    }

    {
        enum class Hamsters { MyHamster = 10, Stranger = -10 };
        constexpr auto names = EnumTraits<Hamsters>::names();
        EXPECT_THAT(names, ElementsAre("Stranger", "MyHamster"));
    }
}

TEST(enumTraits, name) {
    enum class Fruits { Apples = 2, Oranges = 5, Pears = 8, Bananas = 11 };
    EXPECT_EQ(EnumTraits<Fruits>::name(Fruits::Apples), "Apples");
    EXPECT_EQ(EnumTraits<Fruits>::name(Fruits::Oranges), "Oranges");
    EXPECT_EQ(EnumTraits<Fruits>::name(Fruits::Pears), "Pears");
    EXPECT_EQ(EnumTraits<Fruits>::name(Fruits::Bananas), "Bananas");
    EXPECT_EQ(EnumTraits<Fruits>::name(static_cast<Fruits>(42)), "");

    enum class Cities { Prague = -127, Berlin = 126 };
    EXPECT_EQ(EnumTraits<Cities>::name(Cities::Prague), "Prague");
    EXPECT_EQ(EnumTraits<Cities>::name(Cities::Berlin), "Berlin");
    EXPECT_EQ(EnumTraits<Cities>::name(static_cast<Cities>(42)), "");
}

TEST(enumTraits, fromStr) {
    enum class Fruits { Apples = 2, Oranges = 5, Pears = 8, Bananas = 11 };
    EXPECT_EQ(EnumTraits<Fruits>::fromStr("Apples"), Fruits::Apples);
    EXPECT_EQ(EnumTraits<Fruits>::fromStr("Oranges"), Fruits::Oranges);
    EXPECT_EQ(EnumTraits<Fruits>::fromStr("Pears"), Fruits::Pears);
    EXPECT_EQ(EnumTraits<Fruits>::fromStr("Bananas"), Fruits::Bananas);
    EXPECT_EQ(EnumTraits<Fruits>::fromStr("non-existing"), EnumTraits<Fruits>::Invalid);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

