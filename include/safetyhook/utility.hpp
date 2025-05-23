#pragma once

#ifndef SAFETYHOOK_USE_CXXMODULES
#include <algorithm>
#include <cstdint>
#include <optional>
#include <type_traits>
#else
import std.compat;
#endif

#include "safetyhook/common.hpp"

namespace safetyhook {
template <typename T> constexpr void store(uint8_t* address, const T& value) {
    std::copy_n(reinterpret_cast<const uint8_t*>(&value), sizeof(T), address);
}

template <typename T, typename U> constexpr T address_cast(U address) {
    if constexpr (std::is_integral_v<T> && std::is_integral_v<U>) {
        return static_cast<T>(address);
    } else {
        return reinterpret_cast<T>(address);
    }
}

bool SAFETYHOOK_API is_executable(uint8_t* address);

class SAFETYHOOK_API UnprotectMemory {
public:
    UnprotectMemory() = delete;
    ~UnprotectMemory();
    UnprotectMemory(const UnprotectMemory&) = delete;
    UnprotectMemory(UnprotectMemory&& other) noexcept;
    UnprotectMemory& operator=(const UnprotectMemory&) = delete;
    UnprotectMemory& operator=(UnprotectMemory&& other) noexcept;

private:
    friend std::optional<UnprotectMemory> SAFETYHOOK_API unprotect(uint8_t*, size_t);

    UnprotectMemory(uint8_t* address, size_t size, uint32_t original_protection)
        : m_address{address}, m_size{size}, m_original_protection{original_protection} {}

    uint8_t* m_address{};
    size_t m_size{};
    uint32_t m_original_protection{};
};

[[nodiscard]] std::optional<UnprotectMemory> SAFETYHOOK_API unprotect(uint8_t* address, size_t size);

template <typename T> constexpr T align_up(T address, size_t align) {
    const auto unaligned_address = address_cast<uintptr_t>(address);
    const auto aligned_address = (unaligned_address + align - 1) & ~(align - 1);
    return address_cast<T>(aligned_address);
}

template <typename T> constexpr T align_down(T address, size_t align) {
    const auto unaligned_address = address_cast<uintptr_t>(address);
    const auto aligned_address = unaligned_address & ~(align - 1);
    return address_cast<T>(aligned_address);
}
} // namespace safetyhook
