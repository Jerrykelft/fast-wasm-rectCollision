# rectCollision-f32-simd

**Extremely fast rectangle collision detection (AABB vs OBB) using WebAssembly SIMD and float32 with fast-math optimizations.**

This project provides a minimal WebAssembly module compiled from C, implementing high-performance rectangle collision detection using SAT (Separating Axis Theorem) optimized for float32 and SIMD instructions.

---

## Features

- Detects collisions between an axis-aligned rectangle (AABB) and a rotated rectangle (OBB).
- Uses **WebAssembly SIMD** for vectorized calculations.
- Optimized with **float32** and `-ffast-math`.
- Extremely fast: capable of hundreds of thousands of collision checks per frame.
- Minimal footprint: only `.c` and `.wasm` files are needed.
---

## Files

- `rectCollision-f32-simd.c` – C source code implementing the collision detection.
- `rectCollision-f32-simd.wasm` – Compiled WebAssembly module.
---

## Compilation Instructions

The following commands were used to compile and optimize the WebAssembly module:


以下是編譯指令  
```bash
emcc -o rectCollision-f32-simd-tmp.wasm rectCollision-f32-simd.c --no-entry -O3 -msimd128 -flto -ffast-math -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS=\"[\'_rectCollision\']\" -s INITIAL_MEMORY=131072
```

After compilation, the resulting WebAssembly module may include some template runtime support code inserted by Emscripten. In 99% of cases, you don’t need this code. You can use the following commands to decompile the .wasm to .wat, remove unnecessary exported runtime functions to make them dead code, and then run binaryen/wasm-opt to eliminate them, achieving a minimal optimized module.

```bash
wasm2wat rectCollision-f32-simd-tmp.wasm -o rectCollision-f32-simd-tmp.wat

# After pruning unnecessary exports, recompile back to wasm

wat2wasm rectCollision-f32-simd-tmp.wat -o rectCollision-f32-simd-opt-tmp.wasm
wasm-opt rectCollision-f32-simd-opt-tmp.wasm -O3 --enable-nontrapping-float-to-int --enable-simd --dce -o rectCollision-f32-simd.wasm
wasm2wat rectCollision-f32-simd.wasm -o rectCollision-f32-simd.wat
```
