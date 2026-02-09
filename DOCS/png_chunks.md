Reference: https://www.w3.org/TR/PNG-Chunks.html

PNG chunks are structured in a specific manner. Every chunk, including tEXt, is laid out like this:
  - Length (4 bytes): How many bytes are in the data part.
  - Chunk Type (4 bytes): e.g., "IHDR", "IDAT", or "tEXt".
  - Chunk Data (Length bytes): This is where the keyword and JSON live.
  - CRC (4 bytes): A check to make sure the data isn't corrupted.
This enables "jumping" from one chunk to the next to find a target, rather than a typical sliding window approach. 

These 4 bytes, however, must be converted to be used as a C++ integer for operations:
```cpp
uint32_t read_long(std::ifstream& file) {
    unsigned char buf[4];
    file.read(reinterpret_cast<char*>(buf), 4);
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}
```
As seen in: https://gist.github.com/mrousavy/584d1b72cc381f01d6f1af0bbec7343c