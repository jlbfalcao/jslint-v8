
# TODO: V8 has one in python.

h = "const char #{ARGV[0].split('.')[0]}_code[] = {"
c = 0

File.new(ARGV[0], "r").each_byte do |ch|
  h << "0x%X," % ch  
  h << "\n" if c % 16 == 0
  c = c.succ
end
h << "0x00\n";

h << "\n};"
puts h
