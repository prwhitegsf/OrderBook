### To Do

CancelOrder class

Test Sell Market, Sell Stop, Cancel


Add checks and processing for when resting orders cross the bid/ask

eod orders

Clarify relationship between orderbook, price ladder and instrument
- maybe the orderbook is basically the order submission interface
- and then I think I need a user-query interface
    - for getting bid/ask, market depth from instrument
        - need to think about how I'll update depth at price in instrument
    - getting / dispatching trade info from client trade list
    - so I still want to get info out of the price ladder, through orderbook, into instrument


At end of match routine, the match function returns 
- a tuple <order_updates_, updated indices,bid/ask?>
- that can be parsed in orderbook and sent along to instrument and the user-query interface

Update UML and dependency charts

Need to think about all of these queues and memory allocations


Need to revisit permissions
- especially regarding the price ladder

Clean up unneeded orderbook members


