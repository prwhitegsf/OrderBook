## Performance notes

### Two primary goals
- making things faster
- balancing the workload while maintaining data independence among modules for future multi-threading experiments
    - Primarily concerned with
        - submit_order() + accept_orders() (later renamed evaluate_orders())
        - match_orders()

---

2025-09-08 - optimize match(Cancel)

Changes:
- instead of erase_if, I use a lower bound search to find id

Results:
- match times more consistent, particularly in relationship to evaluate order times

---

2025-08-31 - continue reducing memory allocations around matcher and filled orders

Changes:
- combines order_fills and state_updates into a single MatchedOrder object
- adds a custom container: Overwriting Vector (see README for details)
    - replaces MatchedOrders::limit_fills with Overwriting Vector
    - replaces OrderBook::matched_ with Overwriting Vector

Results:
- matched_orders() average time reduced to ~32ns
- profiling gets us close to match_orders() / evaluate_orders() + submit_order() = 50 / 50
- there's a fair amount of variability between runs
    - appears to be a consequence of the number of cancel orders called
    - std::erase_if can take some time as it's currently running linearly
        - especially with gcc
        - because of the way id's are assigned, I could use a lower bound on this

Next:
- optimize cancel orders

---

2025-08-16 - reducing memory allocations around matcher and filled orders

match_orders() currently takes 3 - 4x as long to process an order as evaluate_orders()
- percentage wise: 80/20
- match_orders() requires ~80-85ns on average to process and order

Changes:
- instead of iteratively popping the contents of the order_updates_ and order_fills_ objects to the Record Depot,
    - which requires re-allocating memory when pushing again
- we copy them out to the Record Depot
    - and overwrite the same indices again

Results:
- match_orders() time is roughly cut in half, averaging ~40ns
- consequently when profiling, match_orders() is now only ~1.5 - 2x as long as evaluate_orders()
- percentage wise ~65/35

Next Step
- We've still got more memory allocations to deal with
    - inside of the order_fills object, the limit_fills vector makes a lot of re allocations
    - Furthermore, we instantiate a new order_fills object each time we match a market or market limit order



---

2025-08-14 - initial commit

The general idea is to have the following Orderbook functions take about the same amount of time to do their work
- submit_order() and accept_orders()
- match_orders()

In my initial profiling tests match_orders() is taking ~4x as much time on average as the accept_orders()
- submit_orders() just pushes the order to a queue, so it requires a negligible amount of time at this point
- match_orders time is dominated by memory allocations, so that's the first place to focus