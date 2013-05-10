task :compile do
  Dir.chdir("ext") do
    `make`
  end
end

task :install do
  Dir.chdir("ext") do
    `phpize --clean`
    `phpize`
    `./configure`
    `make`
    `make install`
  end
end
