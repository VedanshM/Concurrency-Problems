# Report : Analysis of different implementations of merge sort

## Normal merge sort

Both halves are sorted recursively till the size of the array is 5 and then selection 
sort is used.  The halves are then merged to get complete sorted array.

## Multi-process merge sort

In this we created two child processes recursively to merge the two halves and parent waited till that. After that parent merged the two halves. Around 2*(no of elements) processes are created in this. To have access to the same array in multiple processes shared memory is used.

## Multi-thread merge sort

Similar as multi-prcesses merge sort but no shared memory is required. And thread creation is less costly than forking.

## Time spent in sorting

Since selection sort is used for array_size <5 , hence analysis only seems right if no of elements is more than 5. In all the cases, the normal merge sort seems to be far better than both other types in terms of time required for sorting by a great margin. According to tests performed on my system (on an average): 

if n = 10:  
    + time taken by multi-process merge sort: x450 times normal mergesort  
    + time taken by mulit-thread merge sort: x200 times normal mergesort
  
if n = 100:  
    + time taken by multi-process merge sort: x240 times normal mergesort  
    + time taken by mulit-thread merge sort: x120 times normal mergesort

if n = 1000:  
    + time taken by multi-process merge sort: x140 times normal mergesort  
    + time taken by mulit-thread merge sort: x70 times normal mergesort  

if n = 5000:  
    + time taken by multi-process merge sort: x250 times normal mergesort  
    + time taken by mulit-thread merge sort: x30 times normal mergesort  

if n = 10000:  
    + time taken by multi-process merge sort: x500 times normal mergesort  
    + time taken by mulit-thread merge sort: x50 times normal mergesort  

## Explanation

There are huge overheads in creating a whole new process and overheads are still there if we create additional threads. In this case, those overheads prove to be significant and overall increased the time taken instead of decreasing it by implementing parallel processing. Also in this due to the fact that multiple sections of the array are processed simulatneously and it may happen that whole array doesn't fit into the cache. Hence cache misses are also expcted in that case, making concurrent algorithms practically much slower.
