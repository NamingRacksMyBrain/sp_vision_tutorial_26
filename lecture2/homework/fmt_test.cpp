#include <fmt/format.h>
#include <fmt/printf.h> // 如果需要使用类似 printf 的功能

int main() {
    // 使用 fmt::format 创建格式化字符串
    std::string formatted = fmt::format("Hello, {}! You are {} years old.", "Alice", 25);
    
    // 使用 fmt::print 直接打印到控制台
    fmt::print("Formatted string: {}\n", formatted);

    return 0;
}