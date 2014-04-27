if File.exists?(".Rakefile") 
  import ".Rakefile"
end

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
  cmd_run("make test")
  sh "cat tests/*.diff; if [ $? -eq 0 ];then exit 1; fi"
end

task :integration do
  if ENV["TRAVIS_PHP_VERSION"].to_f > 5.3 then
    sh "phpenv config-add tests/integration/protocolbuffers.ini"
    sh "sudo make install"
    sh "sudo cp tests/integration/php-fpm.conf ~/.phpenv/versions/$(phpenv version-name)/etc/php-fpm.conf"
    sh "sudo ~/.phpenv/versions/$(phpenv version-name)/sbin/php-fpm"
    sleep 1

    cmd_run("$(phpenv which php) tests/integration/fcgiget.php localhost:9000/home/travis/chobie/php-protocolbuffers/tests/integration/test.php")
    cmd_run("$(phpenv which php) tests/integration/fcgiget.php localhost:9000/home/travis/chobie/php-protocolbuffers/tests/integration/test.php")
  end
end