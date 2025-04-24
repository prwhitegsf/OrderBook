Orderbook
---
---

Intro
- Why: 
  - portfolio project, 
  - interested in markets and performance
  - plan on using it to build up performance testing techniques
  - and gain a better understanding of both the Assembly and CPU/caching operations
  - better understanding of assign / move / copy operations along with RVO in relation to performance
- What
  - loosely based on cme specs
  - Parts
  - module diagram, focus on Orderbook part
  - small example program in main

Functionality
- market and limit only
- assumes stops and hidden orders queued elsewhere and converted
- assuming ID can both 
  - a) be a 4byte int and 
  - b) represent the order's relative position in the queue
    - this could even be done with a simple static counting function


Structure
- Compile Time Polymorphism
- Dependency
- Concepts and requirements
- Order Types
  - Core Order
  - Active Orders
  - Order Updates
- Order Path
- Matcher
  - applied to orderbook via strategy
  - currently holds the DOM/Price Ladder
    - should probably be separated (again)
  - also holds a vector / queue of Order Updates 
    - empties at the end of each trade
- Level
  - Holds the Queued Order Objects
  - starting with a deque 

Printer

Generators