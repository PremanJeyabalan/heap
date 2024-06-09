FetchContent_Declare(catch
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0)
FetchContent_MakeAvailable(catch)

#add_executable(testOrderBook OrderBookTests.cpp ExchangeOrderMatcher.hpp)
#target_link_libraries(testOrderBook PRIVATE
#        exchange Catch2::Catch2 Catch2::Catch2WithMain fmt::fmt)
