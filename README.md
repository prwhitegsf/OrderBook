## Introduction

### Why

This is a toy order book based roughly on the [CME's order specifications for futures](https://cmegroupclientsite.atlassian.net/wiki/spaces/EPICSANDBOX/pages/457216217/Order+Management).

I'm building this portfolio project in order to develop a better understanding of:
- compile time polymorphism and CRTP
- performance testing tools and techniques
- the performance implications of copy, move and assign operations along with RVO
- branching vs conditional moves
- Assembly
- CPU caching / performance with varying amounts of data


### What

#### Functionality

![image](img/Pasted%20image%2020250427093057.png)

A design for the full order processing system might look something like the above.

##### Client API
- receives incoming orders from the Executing Trading Firm
- performs any remaining validity checks (valid price, quantity, no conflicts with existing open orders, etc)
- breaks a ModifyOrder() into its constituent parts
  - a cancel order immediately followed by a new, modified order
- creates a resulting Order object of the appropriate type and pushes it to the Order Queue
- provides a client interface for market and order queries

##### Order Queue
- Queue for incoming orders
- Submits orders to the Order Book

##### Order Book
- holds the Matching Strategy and the DOM (depth of market)
  - The CME assigns a matching algorithm to a given product
  - to facilitate this we use the strategy pattern to implement the matcher
  - the DOM is a container of prices with a container of limit orders at each price
- receives filled order information from the matcher and price information from the DOM
  - then pushes those updates to the appropriate modules

##### Matching Strategy
- Determines exactly how orders are filled
  - see the[ full table of matching algorithms](https://cmegroupclientsite.atlassian.net/wiki/spaces/EPICSANDBOX/pages/457218521/CME+Globex+Matching+Algorithm+Steps)
  - in this project I'm using only the FIFO algorithm, whose description / rules can be found just below the table linked above
- has a match() function overload for each of the accepted order types
- writes to a vector holding order updates as orders are filled
- is a friend class of the DOM, with direct access to pointers to the underlying data structure for the DOM

##### DOM
- Designed around a container of prices, each with a Level data structure that holds the orders at that price level
- In the current design the DOM has a vector with an index for each price
  - But this is an area where I have a few ideas I want to try out
  - the size of the DOM / number of prices is set at compilation
- the DOM class has private data members: pointers to price levels on the vector like the bid and ask
- the matcher accesses the orders through the DOM,
  - it's a friend class that has access to the pointers mentioned above
  - the DOM itself does not operate on the orders
- the DOM also provides a public interface to the Order Book, allowing Order Book to get updated prices, depths, etc

##### Price Levels
- Each price / index of the DOM is a Level class
  - this is another area where I want to try some different structures and techniques
- The Level classes are designed around a data structure to hold the limit orders
- The data members of each order are only altered by the matcher
- But Level manages insertion, removal and search of the orders container

![image](img/Pasted%20image%2020250427092257.png)
##### Instrument
- holds the basic specifications for a trading product
- name, contract expiration, max price, min price, etc
- potentially holds (or links to)
  - the Order Records class - where clients can query open and recent orders
    - in reality, this is more likely to be handled by a DBMS, but we're keeping it simple for now
  - a client interface where a user can get updated price and/or market information

##### Pending Stop Orders and Limit Orders with hidden quantity
- According to the CME
  - Stop Orders do not actually go into the Order Book,
    - they are queued elsewhere until triggered by price movement
    - at which point they are sent to the Order Queue as market orders
  - Limit orders with hidden quantity go to the end of the Order Queue once their display quantity is filled
- The consequence of this is that our Matching Strategy only processes Market, Limit and Cancel Orders

##### Order Types

###### Order
- the core of all other order objects, that is all other order objects wrap this in some way
- contains only three members: id, quantity, price

###### Submitted Orders
As mentioned above the Client API creates an order object from the incoming order. The orders created here contain all of the information about order qualifiers (ie order duration, display quantity).
- The currently supported types for this project are
  - Submitted Buy Limit
  - Submitted Sell Limit
  - Submitted Buy Market
  - Submitted Sell Market
  - Submitted Cancel
- These order objects
  - are pushed to Order Records
  - are pushed to the Order Queue
  - each have a function make queued order
    - when the order gets to the front of the Order Queue, this function is called and it creates a Queued Order

###### Queued Orders

Queued orders are wrappers around the Order struct
- They designate the order as a Buy/Sell Market/Limit order
- The Order struct is an 8-byte object holding id, quantity and price for each order

Queued Orders are:
- emitted from the Order Queue
- passed into the Matching Strategy
- executed at market or placed into a price level orders container as a limit order

###### Order Updates

Order Updates wrap the underlying Order object and adds members for state and fill price

Upon successful submission, execution, fill or partial fill of an order
- the matching engine pushes the updated order information to a container
- at the end of the matching algorithm for an order, this container is is pushed by the Order Book to:
  - the respective queues for stop orders and limit orders with hidden quantity
  - Instrument and Order Records

##### What's actually in this project?

For this project I'm currently focused on the Order Book module from the implementation above which includes
- the matching strategy (in our case, we have a FIFO implementation)
- the DOM
- the Price Levels (currently have a deque implementation)

I also have a basic implementation of the Instrument and Order Records in order to help me check the correctness of the program, but these are quite bare bones at the moment.

As mentioned at the outset, in this phase of the project I'm interested in performance testing around
- Matching Strategy
  - the matching algorithms and variations on implementations
  - the updated orders queue
- DOM
  - the underlying data structure and organization
  - I'm also interested in testing the sorted vector idea in [David Gross's talk at CppCon 2024](https://www.youtube.com/watch?v=sX2nF1fW7kI)
- Price Levels
  - the underlying data structure and methods of keeping it updated
  - I'd like to try using a ring buffer here

##### What's not in this project currently

Queues for orders that are executed later according to some condition including:
- Stop Orders
- Limit Orders with a hidden quantity
- End of day cancellations

Order Qualifiers
- FOK (fill or kill) and FAK (fill and kill)
  - per the CME, these are only available during the market open
  - to me this suggests that the relevant calculations do not happen within the context of the Order Book / Matching Strategy
- Duration
  - while there's a member for this in the Submitted Orders, there's implementation of order cancellation for Day Orders and Good Till Date Orders. These go into the category "Queues for orders that are executed later"
- Minimum Quantity, Market Order with Protection, Stop Order with Protection
  - These orders all require some check against available liquidity within a price range. This could happen as the order enters the Order Queue.
  - Implementing this way would require the Order Queue to maintain it's own DOM-like container which would
    - contain information for prices levels within a specified range of the bid and ask
    - the depth at each price level when all Queued Orders and triggered stops are executed
- Market Limit / Stop Limit
  - The functionality provided by these order types is part of the limit order processing as it's implemented here
  - However, with an Order Queue set up as described above, this functionality could be moved and applied when the order gets pushed to the Order Queue

### Design

![image](img/Pasted%20image%2020250427130155.png)


![image](img/Pasted%20image%2020250427134705.png)



As mentioned before I'm interested both in performance and the ability to easily swap modules for testing.

To that end, the important template classes here (Matcher, Dom, Level) are conceptually defined by their public interfaces. For example, in order for an Order Book to accept a class as a Matcher, the class must have public methods for match(order, dom) and clear_updates().

For the Order Book's Submit Order function, the Queued Order types being submitted are required to be derived from an empty struct, Queued Order Tag, in a variation of the CRTP pattern. Essentially we use overloads of the Matcher's match() function to parse the order type (BuyLimit, SellMarket, etc) instead of a big if / else or (more likely) a switch.

You may be notice that, aside from the average fill price, all of the prices are in integers, while futures prices in the real world are floats and sometimes fractions.
- The idea here is that we'll use two functions in the Instrument class to "translate" the price to / from double representation to integer and vice versa
- The double representation of the price then is more of a user interface convenience