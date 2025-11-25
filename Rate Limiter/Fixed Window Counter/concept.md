Fixed window counter algorithm works as follows:

+  The algorithm divides the timeline into fix-sized time windows and assign a counter for each window.
+ Each request increments the counter by one.
+ Once the counter reaches the pre-defined threshold, new requests are dropped until a new time window starts.

Let us use a concrete example to see how it works. In Figure 1, the time unit is 1 second and the system allows a maximum of 3 requests per second. In each second window, if more than 3 requests are received, extra requests are dropped as shown in Figure 1.

![Figure1](../../../pic/fixedwindowcounter1.png)

Figure 1

A major problem with this algorithm is that a burst of traffic at the edges of time windows could cause more requests than allowed quota to go through. Consider the following case:

![](../../../pic/fixedwindowcounter2.png)

Figure 2

In Figure 2, the system allows a maximum of 5 requests per minute, and the available quota resets at the human-friendly round minute. As seen, there are five requests between 2:00:00 and 2:01:00 and five more requests between 2:01:00 and 2:02:00. For the one-minute window between 2:00:30 and 2:01:30, 10 requests go through. That is twice as many as allowed requests.

## Pros:

+ Memory efficient.
+ Easy to understand.
+ Resetting available quota at the end of a unit time window fits certain use cases.

## Cons:

+ Spike in traffic at the edges of a window could cause more requests than the allowed quota to go through.