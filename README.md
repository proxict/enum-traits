![GitHub](https://img.shields.io/github/license/proxict/enum-traits)

enum-traits
-----------

With enum-traits, you can do the "*impossible*":  
Welcome, enum reflection!

```c++
#include <enum-traits/enum-traits.hpp>
using namespace etraits;

#include <iostream>

int main() {
    enum class Fruits { Apples = -1, Oranges = 1, Pears = 3, Bananas = 5 };
                                                                        // Prints:
    std::cout << "Names:" << std::endl;                                     // Names:
    for (const StaticString& name : EnumTraits<Fruits>::names()) {          //   Apples
        std::cout << "  " << name.c_str() << std::endl;                     //   Oranges
    }                                                                       //   Pears
                                                                            //   Bananas

                                                                        // Prints:
    std::cout << "Values:" << std::endl;                                    // Values:
    for (const Fruits fruit : EnumTraits<Fruits>::values()) {               //   -1
        std::cout << "  " << static_cast<int>(fruit) << std::endl;          //   1
    }                                                                       //   3
                                                                            //   5

    std::cout << "Valid enum: " << EnumTraits<Fruits>::name(Fruits::Pears).c_str() << std::endl; // Prints: "Pears"
    std::cout << "Invalid enum: " << EnumTraits<Fruits>::name(static_cast<Fruits>(-3)).c_str() << std::endl; // Prints: ""

    const Fruits value = EnumTraits<Fruits>::fromStr("Bananas");
    std::cout << "Enum value for Bananas is " << static_cast<int>(value) << std::endl; // Prints: 5

    std::cout << EnumTraits<Fruits>::name(EnumTraits<Fruits>::fromStr("Apples")).c_str() << std::endl; // Prints: "Apples"
}
```

Features
--------
 - C++14
 - Header-only
 - Enum to string
 - String to enum
 - Iterating over enum values
 - Iterating over enum names (automatically reflected names)

Limitations
-----------
 - Only works with `gcc >= 9` and `clang >= 4`. `MSVC` is not yet supported.
 - Enum values have limited range (from -128 to 127 by default). This range can be changed by setting the `ENUM_TRAITS_MIN_ENUM_VALUE` and `ENUM_TRAITS_MAX_ENUM_VALUE` preprocessor macros.
   - Note, however, the bigger the range, the longer the compilation times!
   - This range cannot be extended indefinitely, compilers have template recursion depth limits - see [`-ftemplate-depth`](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Dialect-Options.html) - and there are also OS and hardware limitations to this aspect.
 - Enum names can be up to 64 characters in length by default. You can alter this limit by setting the `ENUM_TRAITS_MAX_NAME_LENGTH` preprocessor macro.

