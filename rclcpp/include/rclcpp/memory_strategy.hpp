// Copyright 2015 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef RCLCPP_RCLCPP_MEMORY_STRATEGY_HPP_
#define RCLCPP_RCLCPP_MEMORY_STRATEGY_HPP_
#include <rclcpp/any_executable.hpp>

namespace rclcpp
{

// TODO move HandleType somewhere where it makes sense
enum class HandleType {subscription_handle, service_handle, client_handle, guard_condition_handle};

namespace executor
{
class Executor;
}

namespace memory_strategy
{

/// Delegate for handling memory allocations while the Executor is executing.
/**
 * By default, the memory strategy dynamically allocates memory for structures that come in from
 * the rmw implementation after the executor waits for work, based on the number of entities that
 * come through.
 */
class MemoryStrategy
{

  friend class executor::Executor;

public:
  RCLCPP_SMART_PTR_DEFINITIONS(MemoryStrategy);

  /// Borrow memory for storing data for subscriptions, services, clients, or guard conditions.
  /**
   * The default implementation stores std::vectors for each handle type and resizes the vectors
   * as necessary based on the requested number of handles.
   * \param[in] The type of entity that this function is requesting for.
   * \param[in] The number of handles to borrow.
   * \return Pointer to the allocated handles.
   */
  virtual void ** borrow_handles(HandleType type, size_t number_of_handles)
  {
    switch (type) {
      case HandleType::subscription_handle:
        if (subscription_handles.size() < number_of_handles) {
          subscription_handles.resize(number_of_handles, 0);
        }
        return static_cast<void **>(subscription_handles.data());
      case HandleType::service_handle:
        if (service_handles.size() < number_of_handles) {
          service_handles.resize(number_of_handles, 0);
        }
        return static_cast<void **>(service_handles.data());
      case HandleType::client_handle:
        if (client_handles.size() < number_of_handles) {
          client_handles.resize(number_of_handles, 0);
        }
        return static_cast<void **>(client_handles.data());
      case HandleType::guard_condition_handle:
        if (number_of_handles > 2) {
          throw std::runtime_error("Too many guard condition handles requested!");
        }
        return guard_cond_handles.data();
      default:
        throw std::runtime_error("Unknown HandleType " + std::to_string(static_cast<int>(type)) +
                ", could not borrow handle memory.");
    }
  }

  /// Return the memory borrowed in borrow_handles.
  /**
   * return_handles should always mirror the way memory was borrowed in borrow_handles.
   * \param[in] The type of entity that this function is returning.
   * \param[in] Pointer to the handles returned.
   */
  virtual void return_handles(HandleType type, void ** handles)
  {
    switch (type) {
      case HandleType::subscription_handle:
        if (handles != subscription_handles.data()) {
          throw std::runtime_error(
                  "tried to return memory that isn't handled by this MemoryStrategy");
        }
        memset(handles, 0, subscription_handles.size());
        break;
      case HandleType::service_handle:
        if (handles != service_handles.data()) {
          throw std::runtime_error(
                  "tried to return memory that isn't handled by this MemoryStrategy");
        }
        memset(handles, 0, service_handles.size());
        break;
      case HandleType::client_handle:
        if (handles != client_handles.data()) {
          throw std::runtime_error(
                  "tried to return memory that isn't handled by this MemoryStrategy");
        }
        memset(handles, 0, client_handles.size());
        break;
      case HandleType::guard_condition_handle:
        if (handles != guard_cond_handles.data()) {
          throw std::runtime_error(
                  "tried to return memory that isn't handled by this MemoryStrategy");
        }
        guard_cond_handles.fill(0);
        break;
      default:
        throw std::runtime_error("Unknown HandleType " + std::to_string(static_cast<int>(type)) +
                ", could not borrow handle memory.");
    }
  }

  /// Provide a newly initialized AnyExecutable object.
  // \return Shared pointer to the fresh executable.
  virtual executor::AnyExecutable::SharedPtr instantiate_next_executable()
  {
    return std::make_shared<executor::AnyExecutable>();
  }

  /// Implementation of a general-purpose allocation function.
  /**
   * \param[in] size Number of bytes to allocate.
   * \return Pointer to the allocated chunk of memory.
   */
  virtual void * alloc(size_t size)
  {
    if (size == 0) {
      return NULL;
    }
    return std::malloc(size);
  }

  /// Implementation of a general-purpose free.
  /**
   * \param[in] Pointer to deallocate.
   */
  virtual void free(void * ptr)
  {
    return std::free(ptr);
  }

  std::vector<rclcpp::subscription::SubscriptionBase::SharedPtr> subs;
  std::vector<rclcpp::service::ServiceBase::SharedPtr> services;
  std::vector<rclcpp::client::ClientBase::SharedPtr> clients;

  std::vector<void *> subscription_handles;
  std::vector<void *> service_handles;
  std::vector<void *> client_handles;
  std::array<void *, 2> guard_cond_handles;
};


MemoryStrategy::SharedPtr create_default_strategy()
{
  return std::make_shared<MemoryStrategy>(MemoryStrategy());
}

}  /* memory_strategy */

}  /* rclcpp */

#endif