#include "winkeyercommandbuffer.h"

WinkeyerCommandBuffer::WinkeyerCommandBuffer(){
  clear();
}

void WinkeyerCommandBuffer::clear(){
  write_ptr = 0;
}

void WinkeyerCommandBuffer::write(uint8_t b){
  if (count() < WK_CMDBUFFER_SIZE){
    buffer[write_ptr++] = b;
  }
}

uint8_t WinkeyerCommandBuffer::read(uint8_t position){
  return buffer[position];
}

uint8_t WinkeyerCommandBuffer::count(){
  return write_ptr;
}
