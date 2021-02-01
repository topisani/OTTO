#include "json.hpp"

#include <fstream>

namespace otto::json {

  value parse_file(const std::filesystem::path& path)
  {
    std::ifstream f;
    std::string s;
    f.open(path);
    f >> s;
    f.close();
    return value::parse(s, nullptr, true, true);
  }

  void write_to_file(const value& v, const std::filesystem::path& path)
  {
    std::ofstream file;
    file.open(path);
    file << v;
    file.close();
  }

} // namespace otto::json
