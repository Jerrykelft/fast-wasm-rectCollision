emsdk_env
emcc -c simd.c -o simd.o -O3 -msimd128 -msse3 -flto -ffast-math -std=c23
em++ -c rectCollision3D.cpp -o rectCollision3D.o -O3 -msimd128 -flto -ffast-math -std=c++23
em++ simd.o rectCollision3D.o -o rectCollision3D.wasm -O3 -msimd128 -flto --no-entry -s INITIAL_MEMORY=131072 -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS="['_get', '_make', '_t1', '_t2']"
wasm2wat rectCollision3D.wasm -o rectCollision3D.wat
# -mrelaxed-simd -s ERROR_ON_UNDEFINED_SYMBOLS=1
Remove-Item -Path "./simd.o"
Remove-Item -Path "./rectCollision3D.o"
