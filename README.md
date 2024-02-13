# parallel-merge-sort
The famous merge sort algorithm's parallel version

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





