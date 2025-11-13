# Fast Rectangle Collision Detection (WebAssembly)

###### Due to the highly simplified new algorithm, the speed benefit of SIMD is now minimal (honestly, almost negligible). We'll stick with it for the time being \:).

A high-performance WebAssembly module for rectangle collision detection, optimized with SIMD instructions.

## Features

- Fast AABB (Axis-Aligned Bounding Box) and OBB (Oriented Bounding Box) collision detection (OBB and OBB will be automatically converted to AABB and OBB)
- SIMD-optimized calculations using WebAssembly SIMD128
- Support for both f32 (single precision) and f64 (double precision) floating-point calculations
- Efficient memory usage and computation

## Exported Functions

The module exports four variants of the collision detection function:

- `rectCollisionf`: 32-bit floating-point collision detection
- `rectCollision`: 64-bit floating-point collision detection

### Function Parameters

Each function takes the following parameters for two rectangles:

```c
(x1, y1, width1, height1, rotation1, x2, y2, width2, height2, rotation2)
```

- `x, y`: Center position of the rectangle
- `width, height`: Dimensions of the rectangle
- `rotation`: Rotation angle in radians

Returns `1` if the rectangles are colliding, `0` otherwise.

## Implementation Details

- Uses SAT (Separating Axis Theorem) for OBB collision detection
- Implements efficient SIMD operations for vector calculations

## Building

1. Please ensure that you have installed the Emscripten SDK and added the SDK root directory to your environment variables.
2. Run the compilation script:

```powershell
powershell -File "./compile.ps1"
```

The script will:
1. Compile separate object files for each variant (f32/f64)
2. Link them into a single WebAssembly module
3. Clean up temporary object files

## Usage Example

```javascript
// Load the WebAssembly module
const wasmModule = await WebAssembly.instantiateStreaming(fetch('rectCollision.wasm'));
const { rectCollisionf } = wasmModule.instance.exports;

// Check collision between two rectangles
const isColliding = rectCollisionf(
    0, 0, 100, 50, Math.PI/4,  // First rectangle: centered at (0,0), 100x50, rotated 45°
    50, 50, 80, 60, 0          // Second rectangle: centered at (50,50), 80x60, no rotation
);
```

## Performance Considerations

- F32 variants are generally faster but less precise
- SIMD operations provide significant speedup for vector calculations

## License

Apache-2.0 License - See LICENSE file for details
