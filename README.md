[![Build Status](https://secure.travis-ci.org/cubox/rankable_graph.png?branch=master)](http://travis-ci.org/cubox/rankable_graph)
rankable_graph
==============

A Ruby [PageRank](http://en.wikipedia.org/wiki/PageRank) like implementation.

[![](http://upload.wikimedia.org/wikipedia/commons/thumb/f/fb/PageRanks-Example.svg/596px-PageRanks-Example.svg.png)](http://en.wikipedia.org/wiki/PageRank)

Description
-----------

This gem is mostly writen in C with a pretty Ruby wrapper.
It's intended to be used for big but not huge graphs, as 
those are better processed with a map-reduce distributed solution.

Usage
-----

```ruby
rankable_graph = RankableGraph.new

#First we draw our directed graph using the link method which receives as parameters two identifiers.   
#The only restriction for the identifiers is that they should be integers.
rankable_graph.link(1234, 4312)
rankable_graph.link(9876, 4312)
rankable_graph.link(4312, 9876)
rankable_graph.link(8888, 4312)

probability_of_following_a_link = 0.85 # The bigger the number, less probability we have to teleport to some random link
tolerance = 0.0001 # the smaller the number, the more exact the result will be but more CPU cycles will be needed

rankable_graph.rank(probability_of_following_a_link, tolerance) do |identifier, rank|
puts "Node #{identifier} rank is #{rank}"
end
```

Which outputs

    Node 1234 rank is 0.0375000014901161
    Node 4312 rank is 0.479941636323929
    Node 9876 rank is 0.445058345794678
    Node 8888 rank is 0.0375000014901161

This ranks represent the probabilities that a certain node will be visited.

If you want to calculate the rank of another graph is recommended to call the clear method and reuse the RankableGraph instance as this frees the used memory without relying in the Ruby garbage collector.
So instead of:

```ruby
rankable_graph = RankableGraph.new

rankable_graph.link(0, 1)
rankable_graph.link(1, 2)
rankable_graph.rank(0.85, 0.0001){ |identifier, rank| puts "Node #{identifier} rank for graph1 is #{rank}" }

rankable_graph = RankableGraph.new

rankable_graph.link(0, 1)
rankable_graph.link(2, 1)
rankable_graph.rank(0.85, 0.0001){ |identifier, rank| puts "Node #{identifier} rank for graph2 is #{rank}" }
```

It's better if you do:

```ruby
rankable_graph = RankableGraph.new

rankable_graph.link(0, 1)
rankable_graph.link(1, 2)
rankable_graph.rank(0.85, 0.0001){ |identifier, rank| puts "Node #{identifier} rank for graph1 is #{rank}" }

rankable_graph.clear

rankable_graph.link(0, 1)
rankable_graph.link(2, 1)
rankable_graph.rank(0.85, 0.0001){ |identifier, rank| puts "Node #{identifier} rank for graph2 is #{rank}" }
```

For more examples please refer to the tests.

Requirements
------------

* [glib2](http://library.gnome.org/devel/glib/2.22/) >= 2.22.2
    
    In Mac `brew install glib`

    In a Linux based on Debian `sudo apt-get install libglib2.0-dev`

Note on Patches/Pull Requests
-----------------------------

* Fork the project.
* Make your feature addition or bug fix.
* Add tests for it. This is important so I don't break it in a
  future version unintentionally.
* Commit, do not mess with rakefile, version, or history.
  (if you want to have your own version, that is fine but bump version in a commit by itself I can ignore when I pull)
* Send me a pull request. Bonus points for topic branches.

Copyright
---------

Copyright (c) 2012 [Cubox](http://cuboxlabs.com). See LICENSE for details.
