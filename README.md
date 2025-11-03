# Fast Rectangle Collision Detection (WebAssembly)

A high-performance WebAssembly module for rectangle collision detection, optimized with SIMD instructions.

## Features

- Fast AABB (Axis-Aligned Bounding Box) and OBB (Oriented Bounding Box) collision detection (OBB and OBB will be automatically converted to AABB and OBB)
- SIMD-optimized calculations using WebAssembly SIMD128
- Support for both f32 (single precision) and f64 (double precision) floating-point calculations
- Fast math optimization options
- Efficient memory usage and computation

## Exported Functions

The module exports four variants of the collision detection function:

- `rectCollisionF32`: 32-bit floating-point collision detection
- `rectCollisionF32fast`: 32-bit floating-point with fast-math optimizations
- `rectCollisionF64`: 64-bit floating-point collision detection
- `rectCollisionF64fast`: 64-bit floating-point with fast-math optimizations

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
- Optimizes common cases (0°, 90°, 180°, 270° rotations) with AABB checks
- Implements efficient SIMD operations for vector calculations
- Uses bounding box pre-check to quickly reject non-colliding cases

## Building

1. Please ensure that you have installed the Emscripten SDK and added the SDK root directory to your environment variables.
2. Run the compilation script:

```powershell
powershell -File "./compile.ps1"
```

The script will:
1. Compile separate object files for each variant (f32/f64, with/without fast-math)
2. Link them into a single WebAssembly module
3. Generate both `.wasm` files
4. Clean up temporary object files

## Usage Example

```javascript
// Load the WebAssembly module
const wasmModule = await WebAssembly.instantiateStreaming(fetch('rectCollision.wasm'));
const { rectCollisionF32 } = wasmModule.instance.exports;

// Check collision between two rectangles
const isColliding = rectCollisionF32(
    0, 0, 100, 50, Math.PI/4,  // First rectangle: centered at (0,0), 100x50, rotated 45°
    50, 50, 80, 60, 0          // Second rectangle: centered at (50,50), 80x60, no rotation
);
```

## Performance Considerations

- F32 variants are generally faster but less precise
- Fast math variants may improve performance at the cost of strict IEEE compliance
- SIMD operations provide significant speedup for vector calculations
- The implementation automatically selects the most efficient algorithm based on rotation angles

## License

Apache-2.0 License - See LICENSE file for details
