#pragma once

#include <atomic>
#include <cstddef>
#include <filesystem>
#include <istream>
#include <optional>
#include <stdexcept>
#include <unordered_map>

namespace swapping {
using ID = std::size_t;

class Swappable {
public:
  virtual ~Swappable() = default;

  virtual void Serialize(std::iostream &fs) = 0;
  virtual void Deserialize(std::iostream &fs) = 0;
};

template <typename T>
concept SwappableDerived = std::is_base_of_v<Swappable, T>;

class Strategy {
protected:
  virtual void *SwapInUntyped(ID id) = 0;

  virtual ID SwapOutUntyped(void *obj) = 0;

public:
  virtual ~Strategy() = default;

  template <SwappableDerived T> T *SwapIn(ID id) {
    return static_cast<T *>(SwapInUntyped(id));
  }

  template <SwappableDerived T> ID SwapOut(T *obj) {
    return SwapOutUntyped(obj);
  }
};

template <SwappableDerived T> class Pointer {
  using value_type = T;
  using pointer_type = T *;

  static std::atomic<ID> s_next_counter;
  static ID NextID() { return ++s_next_counter; }

  ID m_id;
  Strategy *m_strategy;
  std::optional<pointer_type> m_data;

  void EnsureSwappedIn() {
    if (IsSwappedOut()) {
      m_data = m_strategy->SwapIn<T>(m_id);
    }
  }

public:
  Pointer(Strategy *strategy) : m_id{NextID()}, m_strategy{strategy} {}
  Pointer(T *ptr) : m_data{ptr} {}

  bool IsSwappedOut() const { return !m_data.has_value(); }
  bool IsSwappedIn() const { return !IsSwappedOut(); }

  pointer_type GetPointer() const {
    EnsureSwappedIn();
    return m_data.value();
  }

  void SwapIn() {
    if (IsSwappedOut()) {
      m_data = m_strategy->SwapIn<T>(m_id);
    }
  }

  void SwapOut() {
    if (IsSwappedIn()) {
      m_id = m_strategy->SwapOut(m_data.value());
      m_data.reset();
    }
  }
};

template <SwappableDerived T> std::atomic<ID> Pointer<T>::s_next_counter{0};

template <SwappableDerived T>
class StaticSingleTypeFileSwappingStrategy final : public Strategy {
public:
  struct BaseConfiguration {
    std::filesystem::path m_backing_store_path;
  };

  struct ObjectBasedConfiguration final : BaseConfiguration {
    size_t m_max_num_resident_objects;
  };

  struct MemoryBasedConfiguration final : BaseConfiguration {
    size_t m_max_resident_memory_usage_bytes;

    ObjectBasedConfiguration Convert() {
      auto max_num_resident_objects =
          m_max_resident_memory_usage_bytes / sizeof(T);
      if (max_num_resident_objects == 0) {
        throw std::invalid_argument(
            "Max resident memory usage is too small to hold any objects.");
      }
      return ObjectBasedConfiguration{
          .m_backing_store_path = this->m_backing_store_path,
          .m_max_num_resident_objects = max_num_resident_objects};
    }
  };

private:
  ObjectBasedConfiguration m_config;
  std::pmr::unordered_map<ID, T *> m_swapped_objects;

public:
  StaticSingleTypeFileSwappingStrategy(ObjectBasedConfiguration config)
      : m_config{std::move(config)} {}

  StaticSingleTypeFileSwappingStrategy(MemoryBasedConfiguration config)
      : m_config{config.Convert()} {}

  ~StaticSingleTypeFileSwappingStrategy() override = default;

  StaticSingleTypeFileSwappingStrategy(
      const StaticSingleTypeFileSwappingStrategy &) = delete;
  StaticSingleTypeFileSwappingStrategy(
      StaticSingleTypeFileSwappingStrategy &&) = default;
  StaticSingleTypeFileSwappingStrategy &
  operator=(const StaticSingleTypeFileSwappingStrategy &) = delete;
  StaticSingleTypeFileSwappingStrategy &
  operator=(StaticSingleTypeFileSwappingStrategy &&) = default;
};

} // namespace swapping
