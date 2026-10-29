
#include <source_location>
#include <string>
#include <fmt/format.h>

#include <iostream>

#ifndef LOGGER_H
#define LOGGER_H

namespace logger {

inline void logInfo(
    const std::string &message,
    const std::source_location location = std::source_location::current()
) {
    std::cout << fmt::format(
        "[INFO] {}:{}: {}",
        location.file_name(),
        location.line(),
        message
    ) << std::endl;

}

inline void logError(
    const std::string &message,
    const std::source_location location = std::source_location::current()
) {
    std::cout << fmt::format(
        "[ERROR] {}:{}: {}",
        location.file_name(),
        location.line(),
        message
    ) << std::endl;

}

} // namespace logger

#endif
