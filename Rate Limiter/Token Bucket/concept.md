The token bucket algorithm is widely used for rate limiting. It is simple, well understood and commonly used by internet companies. Both Amazon and Stripe use this algorithm to throttle their API requests.

The token bucket algorithm work as follows:

+ A token bucket is a container that has pre-defined capacity. Tokens are put in the bucket at preset rates periodically. Once the bucket is full, no more tokens are added. As shown in Figure 1, the token bucket capacity is 4. The refiller puts 2 tokens into the bucket every second. Once the bucket is full, extra tokens will overflow.

  ![](../../../pic/tokenbucket1.png)
																					**Figure 1**
	
+ Each request consumes one token. When a request arrives, we check if there are enough tokens in the bucket. Figure2 explains how it works.

  + If there are enough tokens, we take one token out for each request, and the request goes through.
  + If there are not enough tokens, the request is dropped.
  ![](../../../pic/tokenbucket2.png)
  																					**Figure 2**

Figure 3 illustrates how token consumption, refill, and rate limiting logic work. In this example, the token bucket size is 4, and the refill rate is 4 per 1 minute.
  ![](../../../pic/tokenbucket3.png)
  																					**Figure 3**

The token bucket algorithm takes two parameters:

+  Bucket size: the maximum number of tokens allowed in the bucket
+  Refill rate: number of tokens put into the bucket every second

How many buckets do we need? This varies, and it depends on the rate-limiting rules. Here are a few examples.

+ It is usually necessary to have different buckets for different API endpoints. For instance, if a user is allowed to make 1 post per second, add 150 friends per day, and like 5 posts per second, 3 buckets are required for each user.
+ If we need to throttle requests based on IP addresses, each IP address requires a bucket.
+ If the system allows a maximum of 10,000 requests per second, it makes sense to have a global bucket shared by all requests.

Pros:

+ The algorithm is easy to implement.
+ Memory efficient.
+ Token bucket allows a burst of traffic for short periods. A request can go through as long as there are tokens left.

Cons:

+ Two parameters in the algorithm are **bucket size** and **token refill rate**. However, it might be challenging to tune them properly.
+ 