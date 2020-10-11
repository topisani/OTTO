#pragma once

#include <yamc_semaphore.hpp>

#include "lib/chrono.hpp"
#include "lib/itc/executor.hpp"

namespace otto::itc {

  struct ExecutorProvider {
    virtual ~ExecutorProvider() = default;

    QueueExecutor& executor() noexcept
    {
      return executor_;
    }

    /// Block thread until all functions enqueued by this thread up to this point
    /// have been executed.
    void sync() noexcept
    {
      auto sem = std::make_unique<yamc::binary_semaphore>(0);
      auto* s = sem.get();
      executor_.execute([sem = std::move(sem)] { sem->release(); });
      s->acquire();
    }

    /// Block thread until all functions enqueued by this thread up to this point
    /// have been executed, with a timeout
    ///
    /// returns true if successfully synced
    [[nodiscard]] bool try_sync(chrono::duration d) noexcept
    {
      auto sem = std::make_unique<yamc::binary_semaphore>(0);
      auto* s = sem.get();
      executor_.execute([sem = std::move(sem)] { sem->release(); });
      return s->try_acquire_for(d);
    }

  private:
    QueueExecutor executor_;
  };

} // namespace otto::itc
