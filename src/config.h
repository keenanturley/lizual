#pragma once

#include <cstdint>
#include <filesystem>

/**
 * Config is for values that should be loaded at start time, not compile time,
 * so they can be configured by the user.
 *
 * Default values are declared here, but will be overridden at load time.
 *
 * Values can be changed at runtime, but may be read by consumers infrequently.
 */
struct config {
  static inline uint32_t update_rate = 60;
};

/**
 * Reads a file that contains "field=value" lines and writes to the static
 * config.
 */
void load_config_from_file(const std::filesystem::path& path);