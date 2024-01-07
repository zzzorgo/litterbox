#ifndef BUFFEREDPRINT_H
#define BUFFEREDPRINT_H

#include <Arduino.h>

class BufferedPrint : public Print {
public:
  // Constructor
  BufferedPrint(size_t bufferSize = 128);

  // Destructor
  ~BufferedPrint();

  // Override the write method from the Print class
  size_t write(uint8_t data) override;

  // Method to get the buffered data
  const char* getBuffer() const;

  // Method to clear the buffer
  void clearBuffer();

private:
  char* buffer;
  size_t bufferSizeLocal;
  size_t position;
};

#endif // BUFFEREDPRINT_H
