(module
  (type (;0;) (func))
  (type (;1;) (func (param i32)))
  (type (;2;) (func (result i32)))
  (type (;3;) (func (param f32 f32 f32)))
  (func (;0;) (type 0)
    nop)
  (func (;1;) (type 1) (param i32)
    local.get 0
    global.set 0)
  (func (;2;) (type 2) (result i32)
    global.get 0)
  (func (;3;) (type 3) (param f32 f32 f32)
    i32.const 1112
    local.get 1
    f32.store
    i32.const 1108
    local.get 0
    f32.store
    i32.const 1100
    local.get 1
    f32.store
    i32.const 1096
    local.get 0
    f32.store
    i32.const 1088
    local.get 1
    f32.store
    i32.const 1084
    local.get 0
    f32.store
    i32.const 1076
    local.get 1
    f32.store
    i32.const 1072
    local.get 0
    f32.store
    i32.const 1064
    local.get 1
    f32.store
    i32.const 1060
    local.get 0
    f32.store
    i32.const 1052
    local.get 1
    f32.store
    i32.const 1048
    local.get 0
    f32.store
    i32.const 1040
    local.get 1
    f32.store
    i32.const 1036
    local.get 0
    f32.store
    i32.const 1028
    local.get 1
    f32.store
    i32.const 1024
    local.get 0
    f32.store
    i32.const 1116
    local.get 2
    f32.const -0x1.95f618p+0 (;=-1.58579;)
    f32.add
    f32.store
    i32.const 1104
    local.get 2
    f32.const -0x1.2bec32p-1 (;=-0.585786;)
    f32.add
    f32.store
    i32.const 1092
    local.get 2
    f32.const -0x1.3504f2p+1 (;=-2.41421;)
    f32.add
    f32.store
    i32.const 1080
    local.get 2
    f32.const -0x1.6a09e6p+0 (;=-1.41421;)
    f32.add
    f32.store
    i32.const 1068
    local.get 2
    f32.const 0x1.6a09e6p+0 (;=1.41421;)
    f32.add
    f32.store
    i32.const 1056
    local.get 2
    f32.const 0x1.3504f2p+1 (;=2.41421;)
    f32.add
    f32.store
    i32.const 1044
    local.get 2
    f32.const 0x1.2bec32p-1 (;=0.585786;)
    f32.add
    f32.store
    i32.const 1032
    local.get 2
    f32.const 0x1.95f618p+0 (;=1.58579;)
    f32.add
    f32.store)
  (func (;4;) (type 2) (result i32)
    i32.const 1024)
  (func (;5;) (type 1) (param i32)
    local.get 0
    if  ;; label = @1
      call 6
      unreachable
    end)
  (func (;6;) (type 0)
    unreachable)
  (table (;0;) 3 3 funcref)
  (memory (;0;) 2 2)
  (global (;0;) (mut i32) (i32.const 66656))
  (export "memory" (memory 0))
  (export "test" (func 3))
  (export "getPtr" (func 4))
  (export "_initialize" (func 0))
  (export "__indirect_function_table" (table 0))
  (export "_emscripten_stack_restore" (func 1))
  (export "emscripten_stack_get_current" (func 2))
  (export "__cxa_increment_exception_refcount" (func 5))
  (elem (;0;) (i32.const 1) func 0 6))
