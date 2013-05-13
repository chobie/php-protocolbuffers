def cmd_run(command)
  print "\#execute #{command}...\n"
  IO.popen(command) {|pipe|
    pipe.each { |line|
      print "+ " + line
    }
  }
end

task :compile do
  cmd_run("make")
end

task :install do
  cmd_run("phpize --clean")
  cmd_run("phpize")
  cmd_run("./configure")
  cmd_run("make")
  cmd_run("make install")
end

task :test do
  cmd_run("make test NO_INTERACTION=1")
end

