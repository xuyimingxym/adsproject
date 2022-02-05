# adsproject
## Problem description
You are required to implement a system to find the n most popular hashtags that appear on social media such as Facebook or Twitter. For the scope of this project hashtags will be given from an input file. 
Basic idea for the implementation is to use a max priority structure to find out the most popular hashtags. 
You must use the following structures for the implementation.
1. Max Fibonacci heap: use to keep track of the frequencies of hashtags. 
2.  Hash table: The key for the hash table is the hashtag, and the value is the pointer to the corresponding node in the Fibonacci heap.
You can assume there will be a large number of hashtags appearing in the stream and you need to perform increase key operation many times. Max Fibonacci heap is recommended because it has an amortized complexity of O(1) for the increase key operation. You should implement all Fibonacci heap functions discussed  in  class.  For  the  hash  table,  you  can  use  an  implementation from any  library  that  your programming languages supports (e.g., STL).
