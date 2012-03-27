version = File.read("VERSION")
spec = Gem::Specification.new do |s|
  s.extensions = FileList["ext/**/extconf.rb"]
  s.name = "rankable_graph"
  s.summary = %Q{A Ruby Pagerank implementation}
  s.description = %Q{A Ruby Pagerank implementation}
  s.email = "dev@cuboxsa.com"
  s.homepage = "http://github.com/cubox/rankable_graph"
  s.authors = ["Daniel Cadenas"]
  s.add_development_dependency "rspec", ">= 1.2.9"
  s.requirements << 'glib2, v2.22.2 or greater'
  s.version = version
  s.files = `git ls-files`.split
end

require 'rake/extensiontask'
Rake::PackageTask.new("rankable_graph", version) do |pkg|
end

Rake::ExtensionTask.new('rankable_graph', spec)

require 'rspec/core/rake_task'
RSpec::Core::RakeTask.new(:spec) do |spec|
end

task :spec => :compile

task :default => :spec
