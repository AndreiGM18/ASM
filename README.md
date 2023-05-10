**Name: Mitran Andrei-Gabriel**
**Group: 313CA**

## Desperate moments - Homework 1

### Description:

* The program implements a generic array that holds different types of elements.
* An element is defined by a header, which specifies its type and the size of the element's data, as well as the
* *actual* data. Its type can be 1, 2 or 3.
* All elements are made up of their header, followed by a string, two numbers and another string.
* The strings represent two names, one who "donates" money and another who receives it. The two numbers represent the
* money split into two banknotes. They can have different values.
* The number's sizes differ based on the element's type:
* type 1 - first and second number: int8_t;
* type 2 - first number: int16_t, second number: int32_t;
* type 3 - first and second number: int32_t.
* The program can do a series of commands: it can insert an element at the end of or at a certain index in arr, print
* one element (specified by its index) or all elements in arr, as well as delete an element specified by its index.
* If a command is invalid, or it does not have enough or valid paramenters, nothing happens (the command is skipped).
* The first element of arr has the index 0.
* If an index is invalid (< 0 or > (number of elements - 1)), usually nothing happens. An exception to the rule is the
* "insert_at" command. If an index > (number of elements - 1) is given, the new element is added at the end of arr.
* When the "insert" or the "insert_at" command is given, a data_structure is created. Then, a jump to the end or the
* specified index occurs and the element is inserted.
* When the "find" command is given, it jumps to the element and prints it. When "print" is given instead, it jumps
* from one element to the next, printing each one of them.
* When the "delete_at" command is given, it jumps to the specified index and deletes the element.
* When an "insert" or a "delete" occurs, the memory of arr gets reallocated accordingly.
* If the program receives the command "exit", it frees all dynamically allocated memory and stops.

### Comments regarding the homework:
* I learned a lot regarding memory allocation and genericity. The jumps performed in arr helped me understand pointers
* better as well.
* When it comes to improving the program, one might create the data_structure more efficiently or perhaps do the jumps
* quicker.
* I wrote the code and the comments in English. I apologize if I was not supposed to do so.

