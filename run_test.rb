#!/usr/bin/env ruby

require 'json'

load_dir = File.dirname(
  File.symlink?(__FILE__) ? File.readlink(__FILE__) : __FILE__
)

file = File.read("#{load_dir}/tests_available.json")

tests_available = JSON.parse(file)

load_dir = Dir.pwd if load_dir == '.'
puts "Running tests from #{load_dir}\n\n"

if ARGV.length < 1
  exec("make clean && make -C #{load_dir}/src/userprog -j 16 check > output 2>&1 && atom outfile && cat outfile | grep FAIL")
else
  test_ = []
  ARGV.each do |arg|
    test_a = tests_available[arg.to_s]
    test_.push(name: arg, test: test_a) if test_a
  end

  dir_made = []

  test_.each do |t|
    dir = t[:test]['dir']
    dir = dir.split('/')
    test_dir = []
    dir.each do |dir_|
      if dir_ != 'build'
        test_dir.push(dir_)
      else
        break
      end
    end
    dir = test_dir.join('/')

    `make -C #{load_dir}/#{dir}` unless dir_made.include? dir

    puts "Running test:\t#{t[:name]}"

    test_a = t[:test]
    puts "Command:\t#{test_a['command']}"
    puts "Dir:\t\t#{test_a['dir']}\n"

    `cd #{load_dir}/#{test_a['dir']} && #{test_a['command']}`
    exit_code = $?.exitstatus

    puts "\nExit code:\t#{exit_code}\n\n"
  end

end
