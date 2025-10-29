emsdk_env
emcc -c rectCollision.c -o rectCollision-f32-fastmath.o -O3 -msimd128 -flto -ffast-math
emcc -c rectCollision.c -o rectCollision-f64-fastmath.o -O3 -msimd128 -flto -ffast-math -DUSE_FLOAT64
emcc -c rectCollision.c -o rectCollision-f32.o -O3 -msimd128 -flto
emcc -c rectCollision.c -o rectCollision-f64.o -O3 -msimd128 -flto -DUSE_FLOAT64

emcc rectCollision-f32-fastmath.o rectCollision-f64-fastmath.o rectCollision-f32.o rectCollision-f64.o -o rectCollision.wasm -O3 -msimd128 -flto --no-entry -s INITIAL_MEMORY=131072 -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS="['_rectCollisionF64fast', '_rectCollisionF64', '_rectCollisionF32fast', '_rectCollisionF32']"
# wasm2wat rectCollision.wasm -o rectCollision.wat

Remove-Item -Path "./rectCollision-f32-fastmath.o"
Remove-Item -Path "./rectCollision-f64-fastmath.o"
Remove-Item -Path "./rectCollision-f32.o"
Remove-Item -Path "./rectCollision-f64.o"
