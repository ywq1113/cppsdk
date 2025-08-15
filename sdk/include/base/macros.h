#pragma once

#define DISALLOW_COPY(Class)     \
  Class(const Class &) = delete; \
  Class &operator=(const Class &) = delete

#define DISALLOW_MOVE(Class) \
  Class(Class &&) = delete;  \
  Class &operator=(Class &&) = delete

#define DISALLOW_COPY_AND_MOVE(Class) \
  DISALLOW_COPY(Class);               \
  DISALLOW_MOVE(Class)
