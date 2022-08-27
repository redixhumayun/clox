When debugging, use the following two expressions to easily convert pointers to arrays in the Watch window

1. `*(uint8_t(*)[20])current->function->chunk->code`
2. `*(Value(*)[10])current->function->chunk->constants->values`

```
== fib ==
0000    2 OP_GET_LOCAL        1
0002   |  OP_CONSTANT         0 2
0004   |  OP_LESS
0005   |  OP_JUMP_IF_FALSE    5 -> 15
0008   |  OP_POP
0009   |  OP_GET_LOCAL        1
0011    3 OP_RETURN
0012   |  OP_JUMP            12 -> 16
0015   |  OP_POP
0016   |  OP_GET_GLOBAL       1 fib
0018   |  OP_GET_LOCAL        1
0020   |  OP_CONSTANT         2 2
0022   |  OP_SUBTRACT
0023   |  OP_CALL             1
0025   |  OP_GET_GLOBAL       3 fib

== <script> ==
0000    6 OP_CONSTANT         1 <fn fib>
0002   |  OP_DEFINE_GLOBAL    0 fib
0004   |  OP_GET_GLOBAL       3 clock
0006   |  OP_CALL             0
0008    7 OP_DEFINE_GLOBAL    2 start
0010   |  OP_GET_GLOBAL       4 fib
0012   |  OP_CONSTANT         5 5
0014   |  OP_CALL             1
0016    8 OP_PRINT
0017   |  OP_GET_GLOBAL       6 clock
0019   |  OP_CALL             0
0021   |  OP_GET_GLOBAL       7 start
0023   |  OP_SUBTRACT
0024   |  OP_PRINT
0025   |  OP_NIL
0026   |  OP_RETURN
```