(module
  (type (;0;) (func))
  (type (;1;) (func (param i32)))
  (type (;2;) (func (result i32)))
  (type (;3;) (func (param f32 f32 f32 f32 f32 f32 f32 f32 f32)))
  (func (;0;) (type 0)
    nop)
  (func (;1;) (type 1) (param i32)
    local.get 0
    global.set 0)
  (func (;2;) (type 2) (result i32)
    global.get 0)
  (func (;3;) (type 2) (result i32)
    i32.const 1024)
  (func (;4;) (type 3) (param f32 f32 f32 f32 f32 f32 f32 f32 f32)
    i32.const 1056
    v128.const i32x4 0x00000000 0x00000000 0x00000000 0x00000000
    local.get 6
    f32x4.replace_lane 0
    local.get 7
    f32x4.replace_lane 1
    local.get 8
    f32x4.replace_lane 2
    v128.store
    i32.const 1040
    v128.const i32x4 0x00000000 0x00000000 0x00000000 0x00000000
    local.get 3
    f32x4.replace_lane 0
    local.get 4
    f32x4.replace_lane 1
    local.get 5
    f32x4.replace_lane 2
    v128.store
    i32.const 1024
    v128.const i32x4 0x00000000 0x00000000 0x00000000 0x00000000
    local.get 0
    f32x4.replace_lane 0
    local.get 1
    f32x4.replace_lane 1
    local.get 2
    f32x4.replace_lane 2
    v128.store)
  (func (;5;) (type 0)
    (local v128 v128 v128)
    i32.const 1056
    i32.const 1024
    v128.load
    local.tee 0
    i32.const 1040
    v128.load
    local.tee 2
    i8x16.shuffle 8 9 10 11 24 25 26 27 0 1 2 3 0 1 2 3
    i32.const 1056
    v128.load
    local.tee 1
    i8x16.shuffle 0 1 2 3 4 5 6 7 24 25 26 27 0 1 2 3
    v128.store
    i32.const 1040
    local.get 0
    local.get 2
    i8x16.shuffle 0 1 2 3 16 17 18 19 4 5 6 7 20 21 22 23
    local.tee 0
    local.get 1
    i8x16.shuffle 8 9 10 11 12 13 14 15 20 21 22 23 0 1 2 3
    v128.store
    i32.const 1024
    local.get 0
    local.get 1
    i8x16.shuffle 0 1 2 3 4 5 6 7 16 17 18 19 0 1 2 3
    v128.store)
  (func (;6;) (type 0)
    (local v128 v128)
    i32.const 1056
    i32.const 1040
    i32.const 1024
    v128.const i32x4 0x00000000 0x00000000 0x00000000 0x00000000
    v128.load32_lane 0
    v128.load32_lane 1
    v128.load32_lane 2
    local.set 0
    i32.const 1060
    i32.const 1044
    i32.const 1028
    v128.const i32x4 0x00000000 0x00000000 0x00000000 0x00000000
    v128.load32_lane 0
    v128.load32_lane 1
    v128.load32_lane 2
    local.set 1
    i32.const 1056
    i32.const 1064
    i32.const 1048
    i32.const 1032
    v128.const i32x4 0x00000000 0x00000000 0x00000000 0x00000000
    v128.load32_lane 0
    v128.load32_lane 1
    v128.load32_lane 2
    v128.store
    i32.const 1040
    local.get 1
    v128.store
    i32.const 1024
    local.get 0
    v128.store)
  (func (;7;) (type 1) (param i32)
    local.get 0
    if  ;; label = @1
      call 8
      unreachable
    end)
  (func (;8;) (type 0)
    unreachable)
  (table (;0;) 3 3 funcref)
  (memory (;0;) 2 2)
  (global (;0;) (mut i32) (i32.const 66608))
  (export "memory" (memory 0))
  (export "get" (func 3))
  (export "make" (func 4))
  (export "t1" (func 5))
  (export "t2" (func 6))
  (export "_initialize" (func 0))
  (export "__indirect_function_table" (table 0))
  (export "_emscripten_stack_restore" (func 1))
  (export "emscripten_stack_get_current" (func 2))
  (export "__cxa_increment_exception_refcount" (func 7))
  (elem (;0;) (i32.const 1) func 0 8)
  (data (;0;) (i32.const 1026) "\80?")
  (data (;1;) (i32.const 1046) "\80?")
  (data (;2;) (i32.const 1066) "\80?"))
