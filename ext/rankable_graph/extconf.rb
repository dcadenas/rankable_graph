require 'mkmf'

unless pkg_config('glib-2.0')
  abort "glib2 not found"
end

create_makefile("rankable_graph")

