The fixed window counter algorithm has a major issue: it allows more requests to go through at the edges of a window. The sliding window log algorithm fixes the issue. It works as follows:

+ The algorithm keeps track of request timestamps. Timestamp data is usually kept in cache, such as sorted sets of Redis.
+ When a new request comes in, remove all the outdated timestamps. Outdated timestamps are defined as those older than the start of the current time window.
+ Add timestamp of the new request to the log.
+  If the log size is the same or lower than the allowed count, a request is accepted. Otherwise, it is rejected.

We explain the algorithm with an example as revealed in Figure 1.

![Figure 1](../../../pic/slidingwindowlog1.png)

Figure  1

In this example, the rate limiter allows 2 requests per minute. Usually, Linux timestamps are stored in the log. However, human-readable representation of time is used in our example for better readability.

+  The log is empty when a new request arrives at 1:00:01. Thus, the request is allowed.
+ A new request arrives at 1:00:30, the timestamp 1:00:30 is inserted into the log. After the insertion, the log size is 2, not larger than the allowed count. Thus, the request is allowed.
+ A new request arrives at 1:00:50, and the timestamp is inserted into the log. After the insertion, the log size is 3, larger than the allowed size 2. Therefore, this request is rejected even though the timestamp remains in the log.
+ A new request arrives at 1:01:40. Requests in the range [1:00:40,1:01:40) are within the latest time frame, but requests sent before 1:00:40 are outdated. Two outdated timestamps, 1:00:01 and 1:00:30, are removed from the log. After the remove operation, the log size becomes 2; therefore, the request is accepted.



## Pros:

+ Rate limiting implemented by this algorithm is very accurate. In any rolling window, requests will not exceed the rate limit.



## Cons:

+ The algorithm consumes a lot of memory because even if a request is rejected, its timestamp might still be stored in memory.