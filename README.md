[![Build Status](https://travis-ci.org/PetterS/easy-IP.png)](https://travis-ci.org/PetterS/monte-carlo-tree-search)

Features
-----------
* Multi-core computation (root parallelization [1]).

Requirements
------------
 * C++11, nothing else.
 * CMake is useful for building.
 * If the compiler support OpenMP it will be used for timing.

Performance
-----------
I evaluate performance when computing the first move for connect-four on an 8-core computer.
With Visual Studio 2012 (64-bit), I get 1.4 million complete games per second.
When using GCC 4.8 (64-bit, Cygwin), I get 27 thousand. I don't know why the difference is this large; both tests use CMake's default release compiler flags.
 
References
----------
1. Chaslot, G. M. B., Winands, M. H., & van Den Herik, H. J. (2008). Parallel monte-carlo tree search. In Computers and Games (pp. 60-71). Springer Berlin Heidelberg.
