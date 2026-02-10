<h1 align="center">cimage</h1><p align="center">A CLI tool to quickly view ComfyUI image generation metadata.</p>

### ComfyUI Image Data

ComfyUI stores image metadata in PNG Chunks (see below). The tEXt chunk presents itself as a A Directed Acyclic Graph (DAG) of nodes. Unfortunately, as ComfyUI's node-based approach allows for a lot of flexibility for users, as well as custom nodes, a typical reversed-DFS from a starting (sink) node is often not feasible to extract metadata. This tool explores how this data might be processed for specific use cases. 

### PNG Chunks

Reference: https://www.w3.org/TR/PNG-Chunks.html

PNG chunks are structured in a specific manner. Every chunk, including tEXt, is laid out like this:
  - Length (4 bytes): How many bytes are in the data part.
  - Chunk Type (4 bytes): e.g., "IHDR", "IDAT", or "tEXt".
  - Chunk Data (Length bytes): This is where the keyword and JSON live.
  - CRC (4 bytes): A check to make sure the data isn't corrupted.

These 4 bytes, however, must be converted to be used as a C++ integer for operations:
```cpp
uint32_t read_long(std::ifstream& file) {
    unsigned char buf[4];
    file.read(reinterpret_cast<char*>(buf), 4);
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}
```
As seen in: https://gist.github.com/mrousavy/584d1b72cc381f01d6f1af0bbec7343c

### Parsing the DAG

To handle the structural limitations of the input JSON data, the extractor iterates through the JSON object and looks at the `inputs` field of every node, regardless of its position in the graph.

- Prompts: We can't rely on finding a node named `CLIPTextEncode`. Instead, `crawl_prompts` checks every string value. If a string is long, contains commas (typical of tag-based prompting), and doesn't look like a filename or UI label, it’s stored in a `std::set` to handle duplicates.

- Assets: The tool scans input values for specific extensions (`.safetensors`, `.ckpt`, `.lora`) or keys like `ckpt_name`. This allows us to catch models used in both standard loaders and custom "advanced" loaders.

- Sampling Parameters: We target specific keys that are ubiquitous across sampler nodes, such as `cfg`, `steps`, `sampler_name`, and `denoise`.


File Logic & Execution Flow

- Jump Logic: `main.cpp` uses `seekg` to leapfrog over the PNG. It reads the 4-byte length, checks the type, and if it isn't `tEXt`, it skips `length + 4 bytes` (jumping the data and the CRC) to hit the next header immediately.

- Metadata Extraction: Once a `tEXt` chunk with the key prompt is found, the raw JSON is handed to `ComfyMetadataExtractor::extract_metadata`.

### Building
1. Create a build directory: `mkdir build && cd build`
2. Configure: `cmake ..`
3. Compile: `make`

This will produce two binaries: `cimage` (the app) and `unit_tests`.

### Usage
Run cimage:
`./cimage path/to/your/image.png`

Run tests:
`./unit_tests`