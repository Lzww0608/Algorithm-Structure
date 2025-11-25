The sliding window counter algorithm is a hybrid approach that combines the fixed window counter and sliding window log. The algorithm can be implemented by two different approaches. We will explain one implementation in this section and provide reference for the other implementation at the end of the section. Figure 1 illustrates how this algorithm
works.

![Figure 1](../../../pic/slidingwindowcounter1.png)

Figure 1

Assume the rate limiter allows a maximum of 7 requests per minute, and there are 5 requests in the previous minute and 3 in the current minute. For a new request that arrives at a 30% position in the current minute, the number of requests in the rolling window is calculated using the following formula:

+ Requests in current window + requests in the previous window * overlap percentage of the rolling window and previous window.
+ Using this formula, we get 3 + 5 * 0.7 = 6.5 request. Depending on the use case, the number can either be rounded up or down. In our example, it is rounded down to 6.

Since the rate limiter allows a maximum of 7 requests per minute, the current request can go through. However, the limit will be reached after receiving one more request.

Due to the space limitation, we will not discuss the other implementation here. Interested readers should refer to the reference material. This algorithm is not perfect. It has pros and cons.

## Pros:

+ It smooths out spikes in traffic because the rate is based on the average rate of the previous window.
+ Memory efficient.

## Cons:

+  It only works for not-so-strict look back window. It is an approximation of the actual rate because it assumes requests in the previous window are evenly distributed. However, this problem may not be as bad as it seems. According to experiments done by Cloudflare, only 0.003% of requests are wrongly allowed or rate limited among 400 million requests.