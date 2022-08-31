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

master      optional-closure-branch
0.0803s     