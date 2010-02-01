require 'rubygems'
require 'rake'

begin
  require 'jeweler'
  Jeweler::Tasks.new do |gem|
    gem.name = "rankable_graph"
    gem.summary = %Q{A Ruby Pagerank implementation}
    gem.description = %Q{A Ruby Pagerank implementation}
    gem.email = "dev@cuboxsa.com"
    gem.homepage = "http://github.com/cubox/rankable_graph"
    gem.authors = ["Daniel Cadenas"]
    gem.add_development_dependency "rspec", ">= 1.2.9"
    gem.extensions = ["ext/extconf.rb"]
    gem.required_ruby_version = '>= 1.9'
    gem.requirements << 'glib2, v2.22.2 or greater'
    # gem is a Gem::Specification... see http://www.rubygems.org/read/chapter/20 for additional settings
  end
rescue LoadError
  puts "Jeweler (or a dependency) not available. Install it with: sudo gem install jeweler"
end

require 'spec/rake/spectask'
Spec::Rake::SpecTask.new(:spec) do |spec|
  spec.libs << 'lib' << 'spec'
  spec.spec_files = FileList['spec/**/*_spec.rb']
end

Spec::Rake::SpecTask.new(:rcov) do |spec|
  spec.libs << 'lib' << 'spec'
  spec.pattern = 'spec/**/*_spec.rb'
  spec.rcov = true
end

task :spec => :check_dependencies

task :default => :spec

require 'rake/rdoctask'
Rake::RDocTask.new do |rdoc|
  version = File.exist?('VERSION') ? File.read('VERSION') : ""

  rdoc.rdoc_dir = 'rdoc'
  rdoc.title = "rankable_graph #{version}"
  rdoc.rdoc_files.include('README*')
  rdoc.rdoc_files.include('lib/**/*.rb')
end
