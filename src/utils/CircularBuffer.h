#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>

/**
 * @brief Generic circular buffer implementation
 * @tparam T Data type to store
 * @tparam SIZE Buffer size
 */
template <typename T, uint8_t SIZE>
class CircularBuffer {
public:
  /**
   * @brief Initialize an empty buffer
   */
  CircularBuffer() : head(0), tail(0), count(0) {}
  
  /**
   * @brief Add an item to the buffer
   * @param item Item to add
   * @return True if item was added successfully
   */
  bool push(const T& item) {
    if (isFull()) {
      // Overwrite oldest data if buffer is full
      tail = (tail + 1) % SIZE;
    } else {
      count++;
    }
    
    buffer[head] = item;
    head = (head + 1) % SIZE;
    
    return true;
  }
  
  /**
   * @brief Get the oldest item from the buffer
   * @param item Reference to store the item
   * @return True if an item was retrieved
   */
  bool pop(T& item) {
    if (isEmpty()) {
      return false;
    }
    
    item = buffer[tail];
    tail = (tail + 1) % SIZE;
    count--;
    
    return true;
  }
  
  /**
   * @brief Peek at an item without removing it
   * @param index Index from oldest (0) to newest (count-1)
   * @param item Reference to store the item
   * @return True if an item was retrieved
   */
  bool peek(uint8_t index, T& item) const {
    if (index >= count) {
      return false;
    }
    
    uint8_t pos = (tail + index) % SIZE;
    item = buffer[pos];
    
    return true;
  }
  
  /**
   * @brief Get the newest item
   * @param item Reference to store the item
   * @return True if an item was retrieved
   */
  bool peekNewest(T& item) const {
    if (isEmpty()) {
      return false;
    }
    
    uint8_t pos = (head == 0) ? SIZE - 1 : head - 1;
    item = buffer[pos];
    
    return true;
  }
  
  /**
   * @brief Get the oldest item
   * @param item Reference to store the item
   * @return True if an item was retrieved
   */
  bool peekOldest(T& item) const {
    if (isEmpty()) {
      return false;
    }
    
    item = buffer[tail];
    
    return true;
  }
  
  /**
   * @brief Clear the buffer
   */
  void clear() {
    head = 0;
    tail = 0;
    count = 0;
  }
  
  /**
   * @brief Check if the buffer is empty
   * @return True if empty
   */
  bool isEmpty() const {
    return count == 0;
  }
  
  /**
   * @brief Check if the buffer is full
   * @return True if full
   */
  bool isFull() const {
    return count == SIZE;
  }
  
  /**
   * @brief Get the number of items in the buffer
   * @return Item count
   */
  uint8_t size() const {
    return count;
  }
  
  /**
   * @brief Get the maximum capacity of the buffer
   * @return Buffer capacity
   */
  uint8_t capacity() const {
    return SIZE;
  }

private:
  T buffer[SIZE];
  uint8_t head;
  uint8_t tail;
  uint8_t count;
};

#endif // CIRCULAR_BUFFER_H 