require File.expand_path(File.dirname(__FILE__) + '/spec_helper')

class Float
  # 0.666666666 -> 66.7
  def to_percentage
    100 * (self * (10 ** 3)).round / (10 ** 3).to_f
  end
end

def assert_rank(rankable_graph, expected_rank)
  rankable_graph.rank(0.85, 0.0001){|label, rank|  rank.to_percentage.should == expected_rank[label]}
end

describe RankableGraph do
  it "should be possible to recalculate the ranks after a new link is added" do
    rankable_graph = RankableGraph.new
    rankable_graph.link(0, 1)
    assert_rank(rankable_graph, { 0 => 35.1, 1 => 64.9 })
    rankable_graph.link(1, 2)
    assert_rank(rankable_graph, { 0 => 18.4, 1 => 34.1, 2 => 47.4 })
  end

  it "should be possible to clear the graph" do
    rankable_graph = RankableGraph.new
    rankable_graph.link(0, 1)
    rankable_graph.link(1, 2)
    rankable_graph.clear
    rankable_graph.link(0, 1)
    assert_rank(rankable_graph, { 0 => 35.1, 1 => 64.9 })
  end

  it "should not raise an exception when calculating the rank of an empty graph" do
    rankable_graph = RankableGraph.new
    lambda do
      rankable_graph.rank(0.85, 0.0001){|label, rank| raise "This should not be raised!" }
    end.should_not raise_error
  end

  it "should return correct results when having a dangling node" do
    rankable_graph = RankableGraph.new
    #node 2 is a dangling node because it has no outbound links
    rankable_graph.link(0, 2)
    rankable_graph.link(1, 2)

    expected_rank = {
      0 => 21.3,
      1 => 21.3,
      2 => 57.4
    }

    assert_rank(rankable_graph, expected_rank)
  end

  it "should not change the graph when adding the same link many times" do
    rankable_graph = RankableGraph.new
    rankable_graph.link(0, 2)
    rankable_graph.link(0, 2)
    rankable_graph.link(0, 2)
    rankable_graph.link(1, 2)
    rankable_graph.link(1, 2)

    expected_rank = {
      0 => 21.3,
      1 => 21.3,
      2 => 57.4
    }

    assert_rank(rankable_graph, expected_rank)
  end

  it "should return correct results for a star graph" do
    rankable_graph = RankableGraph.new
    rankable_graph.link(0, 2)
    rankable_graph.link(1, 2)
    rankable_graph.link(2, 2)

    expected_rank = {
      0 => 5,
      1 => 5,
      2 => 90,
    }

    assert_rank(rankable_graph, expected_rank)
  end

  it "should be uniform for a circular graph" do
    rankable_graph = RankableGraph.new
    rankable_graph.link(0, 1)
    rankable_graph.link(1, 2)
    rankable_graph.link(2, 3)
    rankable_graph.link(3, 4)
    rankable_graph.link(4, 0)

    expected_rank = {
      0 => 20,
      1 => 20,
      2 => 20,
      3 => 20,
      4 => 20
    }

    assert_rank(rankable_graph, expected_rank)
  end

  it "should return correct results for a converging graph" do
    rankable_graph = RankableGraph.new
    rankable_graph.link(0, 1)
    rankable_graph.link(0, 2)
    rankable_graph.link(1, 2)
    rankable_graph.link(2, 2)

    expected_rank = {
      0 => 5,
      1 => 7.1,
      2 => 87.9
    }

    assert_rank(rankable_graph, expected_rank)
  end

  it "should correctly reproduce the wikipedia example" do
    #http://en.wikipedia.org/wiki/File:PageRanks-Example.svg
    rankable_graph = RankableGraph.new
    rankable_graph.link(1, 2)
    rankable_graph.link(2, 1)
    rankable_graph.link(3, 0)
    rankable_graph.link(3, 1)
    rankable_graph.link(4, 3)
    rankable_graph.link(4, 1)
    rankable_graph.link(4, 5)
    rankable_graph.link(5, 4)
    rankable_graph.link(5, 1)
    rankable_graph.link(6, 1)
    rankable_graph.link(6, 4)
    rankable_graph.link(7, 1)
    rankable_graph.link(7, 4)
    rankable_graph.link(8, 1)
    rankable_graph.link(8, 4)
    rankable_graph.link(9, 4)
    rankable_graph.link(10, 4)

    expected_rank = {
      0 => 3.3,  #a
      1 => 38.4, #b
      2 => 34.3, #c
      3 => 3.9,  #d
      4 => 8.1,  #e
      5 => 3.9,  #f
      6 => 1.6,  #g
      7 => 1.6,  #h
      8 => 1.6,  #i
      9 => 1.6,  #j
      10 => 1.6   #k
    }

    assert_rank(rankable_graph, expected_rank)
  end
end
