#require 'rubygems'
$LOAD_PATH.unshift(File.join(File.dirname(__FILE__), 'ext'))

require 'ext/rankable_graph'

require "benchmark"
include Benchmark

n = 1000000
bmbm(12) do |test|
    r = RankableGraph.new
    srand(5)
    (0..(n-1)).map do |i|
      #each node has an average of 30 links
      rand(60).times do
        j = rand(n)
        #first three nodes are more linked to than the rest
        r.link(i, (j > 800000 ? rand(3) : j))
      end
    end

  test.report("c:")    do
    result = []
    r.rank(0.85, 0.001){|key, val| result << [key, val]}
    puts "7 first values are #{result[0..6].map{|(k,v)| "[#{k}]=#{"%.4f" % (v * 100)}, "}}"
  end
end



