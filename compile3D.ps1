emsdk_env
emcc -c wasm_simd128++.c -o wasm_simd128++.o -O3 -msimd128 -msse3 -flto -ffast-math
em++ -c rectCollision3D.cpp -o rectCollision3D.o -O3 -msimd128 -flto -ffast-math
em++ wasm_simd128++.o rectCollision3D.o -o rectCollision3D.wasm -O3 -msimd128 -flto --no-entry -s INITIAL_MEMORY=131072 -s STANDALONE_WASM=1 -s EXPORTED_FUNCTIONS="['_test1', '_test2']"
wasm2wat rectCollision3D.wasm -o rectCollision3D.wat

Remove-Item -Path "./wasm_simd128++.o"
Remove-Item -Path "./rectCollision3D.o"
