#pragma once
#include <string>
#include <string_view>

namespace metaforce {
/**
 * @brief Per-platform file store resolution
 */
class FileStoreManager {
  std::string m_domain;
  std::string m_storeRoot;

public:
  FileStoreManager(std::string_view domain);
  std::string_view getDomain() const { return m_domain; }
  /**
   * @brief Returns the full path to the file store, including domain
   * @return Full path to store e.g /home/foo/.hecl/bar
   */
  std::string_view getStoreRoot() const { return m_storeRoot; }
};
}