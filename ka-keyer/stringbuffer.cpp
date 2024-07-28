#include "stringbuffer.h"

StringBuffer::StringBuffer(){
  clear();
}

void StringBuffer::clear(){
  write_ptr = 0;
  buffer[write_ptr] = '\0';
}

void StringBuffer::write(char c){
  if (write_ptr < (MAX_MORSE_LENGTH)){
    buffer[write_ptr++] = c;
    buffer[write_ptr] = '\0';
  }
}

char* StringBuffer::read(){
  return buffer;
}

uint8_t StringBuffer::count(){
  return write_ptr;
}

