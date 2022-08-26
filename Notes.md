When debugging, use the following two expressions to easily convert pointers to arrays in the Watch window

1. `*(uint8_t(*)[20])current->function->chunk->code`
2. `*(Value(*)[10])current->function->chunk->constants->values`