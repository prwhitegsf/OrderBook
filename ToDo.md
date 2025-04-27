### To Do

Split up implementation and headers where appropriate

Performance exploration w/ matcher and levels
- effects of number of prices on performance
- effects of number of orders / level on performance
- looking at dom and level config / data structures

If I decide to keep going:

Order Queue
Stop Queue
Limit orders with hidden qty queue

Potentially
- implement an Order Queue that 
  - maintains a container of the available liquidity, 
    - including queued stop orders and orders in the Order Queue itself
  - can then process things like 
    - market orders with protection
    - market limit orders
  - without sticking a bunch of branching into Matcher