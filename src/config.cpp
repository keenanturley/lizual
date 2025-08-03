#include "config.h"

#include <SDL3/SDL_log.h>

#include <cstring>
#include <fstream>
#include <string>

void load_config_from_file(const std::filesystem::path& path) {
  std::ifstream config_file(path);
  if (!config_file.is_open() || config_file.fail()) {
    SDL_Log(
      "config: Failed to open config file at path: %s", path.string().c_str()
    );
    return;
  }

  size_t line_number = 0;
  static constexpr size_t LINE_BUFFER_LENGTH = 512;
  char line[LINE_BUFFER_LENGTH] = {};

  while (config_file.getline(
    line, static_cast<std::streamsize>(LINE_BUFFER_LENGTH * sizeof(char))
  )) {
    // Error check
    if (config_file.fail()) {
      SDL_Log(
        "config: Failed to read line %zu of config file: %s",
        line_number,
        path.string().c_str()
      );
      return;
    }

    size_t i = 0;

    // Skip leading whitespace
    for (; i < LINE_BUFFER_LENGTH && line[i] != '\0' && line[i] == ' '; ++i);
    size_t field_start = i;

    // Find the first equal sign or white-space
    for (; i < LINE_BUFFER_LENGTH && line[i] != '\0' && line[i] != '=' &&
           line[i] != ' ';
         ++i);
    if (i >= LINE_BUFFER_LENGTH || line[i] == '\0') { continue; }
    size_t field_length = i - field_start;

    // Advance to equal sign and consume it
    for (; i < LINE_BUFFER_LENGTH && line[i] != '\0' && line[i] != '='; ++i);
    if (i >= LINE_BUFFER_LENGTH || line[i] == '\0') { continue; }
    ++i;

    // Advance past white space
    for (; i < LINE_BUFFER_LENGTH && line[i] != '\0' && line[i] == ' '; ++i);
    if (i >= LINE_BUFFER_LENGTH || line[i] == '\0') { continue; }
    char* value = line + i;

    // Read value in as a pointer inside line, null-terminate it
    for (; i < LINE_BUFFER_LENGTH && line[i] != '\0' && line[i] != ' '; ++i);
    if (i >= LINE_BUFFER_LENGTH) { continue; }
    line[i] = '\0';

    // Switch on field name, set value
    if (strncmp(line + field_start, "update_rate", field_length) == 0) {
      try {
        config::update_rate = static_cast<uint32_t>(std::stoi(value));
      } catch (const std::exception& e) {
        SDL_Log(
          "config: Invalid value (%s) for field (%s) in config file: %s",
          value,
          std::string(line + field_start, field_length),
          path.string().c_str()
        );
        continue;
      }
    } else {
      SDL_Log(
        "config: Found invalid field name (%s) in config file: %s",
        std::string(line + field_start, field_length),
        path.string().c_str()
      );
    }

    ++line_number;
  }
}