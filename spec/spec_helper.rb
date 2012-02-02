$LOAD_PATH.unshift(File.dirname(__FILE__))
$LOAD_PATH.unshift(File.join(File.dirname(__FILE__), '..', 'lib'))
require 'rankable_graph'
require 'rspec'
require 'rspec/autorun'

RSpec.configure do |config|
end
