When debugging, use the following two expressions to easily convert pointers to arrays in the Watch window

1. `*(uint8_t(*)[20])current->function->chunk->code`
2. `*(Value(*)[10])current->function->chunk->constants->values`

## Using Closures Vs Functions
One of the challenges in the book is about using closures to wrap functions only where upvalues are required. The purpose of this optimisation is performance.
To benchmark, the function that will be used is:

```
Function without closures
fun testFunctionWithoutClosures() {
    var a = 1 + 2;
    if (a == 3) {
        print "Correct answer";
    } else {
        print "Incorrect answer";
    }
}

var start = clock();
for (var i = 0; i < 1000; i = i + 1) {
  testFunctionWithoutClosures();
}
var t = clock() - start;
print t;

Function with closures

fun testFunctionWithClosures() {
  var local = "local";
  fun closure() {
    fun innerClosure() {
      print local;
    }
    return innerClosure;
  }
  return closure;
}

var start = clock();
for (var i = 0; i < 1000; i = i + 1) {
    var closure = testFunctionWithClosures();
    var innerClosureFunction = closure();
    innerClosureFunction();
}
var t = clock() - start;
print t;
```

### Results Of Running Tests

testFunctionWithoutClosures()

master      optional-closure-branch   Percentage Difference
0.0803s     0.0603s                   24.9066% decrease


testFunctionWithClosures()
master      optional-closures-branch  Percentage Difference
0.080893s   0.083719s                 3.493503% increase

Running functions without closures is a tiny bit faster without the overhead of the closures wrapper. However, running functions that involves closures is slightly slower when incorporating the conditional logic of wrapping the functions in closures or not. This is probably what accounts for the 3.4935% increase.

### Garbage Collection

Example programs for testing garbage collection:

#### Example 1

This example is for globally defined variables.

```
var t = "Hello World";
print t;
delete t;
```

#### Example 2

This example is for locally defined variables like those within a function. In this example, when the delete keyword is encountered, the <fn test> object should be deallocated from memory and all variables it references should have their counter decremented. 

When a function object is deallocated from memory, it should also deallocate the constants array, the function name represented as an `ObjString*`.

```
fun test() {
    var t = "Hello World";
    print t;
}

test();

delete test;
```

#### Example 3

This example is for deallocating closures and upvalues that are defined as a result of nested functions.

When the delete operation is encountered, and the outer function is being deallocated, care needs to be taken to reduce the references to the inner function as well. Important to note that the objects defined within the `test` function are not explicitly deallocated, but the ref count for these objects is decremented.

When a closure is deallocated from memory, it should also deallocate the wrapped function and the upvalues defined within the closure.
```
fun test() {
    var t = "Hello World";
    fun innerTest() {
        print t;
    }
    innerTest();
}

test();
delete test;
```

#### Example 4

This example is for block scope. When the block scope ends you want the objects defined within the scope to be deallocated if no further references to them remain.

{
    var t = "Hello World";
    var s = t;
    print t;
}

When the block scope ends, the VM will emit two OP_POP instructions that will remove the "Hello World" object from the stack. When these instructions are encountered in the VM, the ref count for the object needs to be decremented. 

### General Questions

Are there any instances in which a function or closure can go out of scope which doesn't involve explicitly calling the delete operator? Identifying this question will help me find bugs in the garbage collection implementation. 