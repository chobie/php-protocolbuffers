def cmd_run(command)
  print "\#execute #{command}...\n"
  IO.popen(command) {|pipe|
    pipe.each { |line|
      print "+ " + line
    }
  }
end

task :package do
  cmd_run('php -ddate.timezone="Asia/Tokyo" pyrus.phar pickle -n protocolbuffers pecl.php.net php-protocolbuffers')
end

task :compile do
  cmd_run("phpize")
  cmd_run("./configure")
  cmd_run("make")
end

task :install do
  cmd_run("make install")
end

desc "run php test cases"
task :test do
  ENV["TESTS"] = "--show-diff -q"
  sh "make test"
  sh "cat tests/*.diff; if [ $? -eq 0 ];then exit 1; fi"
end

