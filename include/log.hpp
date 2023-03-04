#ifndef MEMPOOL_LOG_HPP
#define MEMPOOL_LOG_HPP

#include <fmt/format.h>
#include <iostream>

#define LOG(msg) \
    do { \
        auto format = fmt::format("{}:{}: {}\n", __FILE__, __LINE__, msg); \
        std::cout << format; \
    } while (false)

#define LOG_AND_THROW(exception, msg) \
    do { \
        LOG(msg); \
        throw exception(msg); \
    } while (false)

#endif // MEMPOOL_LOG_HPP
