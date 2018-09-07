## This library has been merged into [monolith](unidecode) and will not be updated further here.

[![Build Status](https://travis-ci.org/PetterS/monte-carlo-tree-search.png)](https://travis-ci.org/PetterS/monte-carlo-tree-search)

The search algorithm is quite fast and seems to work pretty well.

Features
-----------
* Multi-core computation (root parallelization [1]).
* Available games:
  * Connect four (text-based)
  * Nim (text-based)

Requirements
------------
 * C++11, nothing else, for the actual search algorithm.
 * CMake is useful for building.
 * If the compiler support OpenMP it will be used for timing.
 * A graphical Go game is available if Cinder is found.

Performance
-----------
I evaluate performance when computing the first move for connect-four on an 8-core computer.
With Visual Studio 2012 (64-bit), I get 1.7 million complete games per second.

References
----------
1. Chaslot, G. M. B., Winands, M. H., & van Den Herik, H. J. (2008). Parallel monte-carlo tree search. In Computers and Games (pp. 60-71). Springer Berlin Heidelberg.
