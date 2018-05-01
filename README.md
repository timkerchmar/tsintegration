# TSIntegration

Get a full screen OpenGL surface, receiving touches and accelerometer input, with a separate rendering thread on Windows, Android, OSX, iOS.

See the master branch readme for more details.
 
## Emscripten

There is a new folder, emscripten. If I sh build.sh and open the html in a browser window, it fails in generated initialization code.

```
// Allocate memory for thread-local storage.
var tlsMemory = allocate(128 * 4, "i32*", ALLOC_STATIC);
for (var i = 0; i < 128; ++i) HEAPU32[tlsMemory/4+i] = 0;
Atomics.store(HEAPU32, (PThread.mainThreadBlock + 116 ) >> 2, tlsMemory); // Init thread-local-storage memory array. <-- ReferenceException here, Atomics not defined
Atomics.store(HEAPU32, (PThread.mainThreadBlock + 52 ) >> 2, PThread.mainThreadBlock); // Main thread ID.
Atomics.store(HEAPU32, (PThread.mainThreadBlock + 56 ) >> 2, PROCINFO.pid); // Process ID.
```