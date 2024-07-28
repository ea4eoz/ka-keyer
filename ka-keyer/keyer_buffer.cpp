#include "keyer_buffer.h"

#define KEYERBUFFER_MASK KEYERBUFFER_SIZE

KeyerBuffer::KeyerBuffer(){
  clear();
}

void KeyerBuffer::clear(){
  write_ptr = 0;
  read_ptr = 0;
}

void KeyerBuffer::write(uint8_t data){
  // Check for at least one byte free
  if (count() < KEYERBUFFER_SIZE){
    buffer[write_ptr++] = data;
    write_ptr &= KEYERBUFFER_MASK;
  }
}

void KeyerBuffer::write(uint8_t data_1, uint8_t data_2){
  // Check for at least two bytes free
  if (count() < (KEYERBUFFER_SIZE - 1)){
    write(data_1);
    write(data_2);
  }
}

uint8_t KeyerBuffer::read(){
  uint8_t data = buffer[read_ptr++];
  read_ptr &= KEYERBUFFER_MASK;
  return data;
}

uint8_t KeyerBuffer::count(){
  return (write_ptr - read_ptr) & KEYERBUFFER_MASK;
}
