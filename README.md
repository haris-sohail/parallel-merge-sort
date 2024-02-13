# parallel-merge-sort
The famous merge sort algorithm's parallel version

## Compiling and Running the Program

To compile the `main.cpp` file and run the program, follow these steps:

1. Open a terminal window.
2. Navigate to the directory containing the `main.cpp` file.
3. Run the following command to compile the program:

    ```bash
    g++ -g main.cpp -o program
    ```

   This command compiles the `main.cpp` file and generates an executable named `program`.

4. After successful compilation, execute the program by running the following command:

    ```bash
    ./program
    ```

   This command executes the compiled program, allowing you to observe its behavior.

Make sure you have `g++` installed on your system. If not, you can install it by following the instructions provided for your operating system.


# Working
![alt text](https://github.com/haris-sohail/parallel-merge-sort/blob/main/assets/merge_sort.png)
## Serial function `executeSerial()`

- Read a list of numbers from file and store them in an array
- Insert the individual numbers into a linked list,
- Sort the linked list using merge sort

## Parallel Function `executeParallel()`

- Read the list of numbers from file and store them in an array
- Insert the individual numbers into a linked list **parallely**
- Divide the list into 8 portions
- Combine 8 portions into 4 sorted lists **parallely**
- Combine 4 portions into 2 sorted lists **parallely**
- Merge 2 sorted lists into a single resultant list





