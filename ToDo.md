### To Do



After removing the dom module from fifo I need to:
- Figure out how to make Fifo a friend class of the MidLadder
  - so that it's member iterators can be private
- Re-enable matcher match() requirement, passing in a dom
- clean up the executing functions in matcher



Basic Tests for Order Records
- will need updating

Documentation

The way market order execution is handled across two functions both
- kinda makes sense
- is awkward
- but maybe wait until I start messing around with performance stuff anyway

Performance exploration w/ matcher and levels
- effects of number of prices on performance
- effects of number of orders / level on performance
- looking at dom and level config / data structures

