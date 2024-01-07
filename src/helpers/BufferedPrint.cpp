#include "BufferedPrint.h"

BufferedPrint::BufferedPrint(size_t bufferSize) {
  buffer = new char[bufferSize];
  bufferSizeLocal = bufferSize;
  clearBuffer();
}

BufferedPrint::~BufferedPrint() {
  delete[] buffer;
}

size_t BufferedPrint::write(uint8_t data) {
  if (position < bufferSizeLocal - 1) {
    buffer[position++] = data;
    buffer[position] = '\0';  // Null-terminate the buffer
    return 1;  // Success
  }
  return 0;  // Buffer is full
}

const char* BufferedPrint::getBuffer() const {
  return buffer;
}

void BufferedPrint::clearBuffer() {
  position = 0;
  buffer[0] = '\0';  // Null-terminate the buffer
}