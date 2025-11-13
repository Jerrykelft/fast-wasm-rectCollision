emsdk_env
emcc -c simd.c -o simd.o -O3 -msimd128 -msse3 -flto -std=c23
em++ -c rectCollision2D.cpp -o rectCollision.o -O3 -msimd128 -flto -std=c++23

emcc simd.o rectCollision.o -o rectCollision2D.wasm -O3 -msimd128 -flto --no-entry -s INITIAL_MEMORY=131072 -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS="['_rectCollisionf', '_rectCollision']"
wasm2wat rectCollision2D.wasm -o rectCollision2D.wat

Remove-Item -Path "./simd.o"
Remove-Item -Path "./rectCollision.o"
