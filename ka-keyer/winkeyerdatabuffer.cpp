#include "winkeyerdatabuffer.h"

#define WK_DATABUFFER_MASK WK_DATABUFFER_SIZE

WinkeyerDataBuffer::WinkeyerDataBuffer(){
  clear();
}

void WinkeyerDataBuffer::clear(){
  read_ptr = 0;
  write_ptr = 0;
  buffer_mode = WK_DATABUFFER_MODE_NORMAL;
  resetCmd16Ptr();
}

void WinkeyerDataBuffer::resetCmd16Ptr(){
  cmd16_ptr = 0;
}

void WinkeyerDataBuffer::write(uint8_t data){
  switch (buffer_mode){
    case WK_DATABUFFER_MODE_NORMAL:
      buffer[write_ptr++] = data;
      write_ptr &= WK_DATABUFFER_MASK;
    break;

    case WK_DATABUFFER_MODE_OVERWRITE:
    case WK_DATABUFFER_MODE_APPEND:
      buffer[cmd16_ptr++] = data;
      cmd16_ptr &= WK_DATABUFFER_MASK;
    break;
  }

  if (cmd16_ptr > write_ptr){
    write_ptr = cmd16_ptr;
  }
}

uint8_t WinkeyerDataBuffer::read(){
  uint8_t data = buffer[read_ptr++];
  read_ptr &= WK_DATABUFFER_MASK;
  return data;
}

uint8_t WinkeyerDataBuffer::count(){
  return (write_ptr - read_ptr) & WK_DATABUFFER_MASK;
}

void WinkeyerDataBuffer::bufferBackSpace(){
  if (count()){
    write_ptr = (write_ptr - 1) & WK_DATABUFFER_MASK;
  }
}

void WinkeyerDataBuffer::setCmd16Ptr(uint8_t p){
  if (p){
    cmd16_ptr = p + 1; // ??? N1MM
  } else {
    cmd16_ptr = 0;
  }
}

void WinkeyerDataBuffer::setMode(uint8_t mode){
  buffer_mode = mode;
}