#!/usr/bin/env ruby
require 'pty'

sig2esc = {
  'INT'  => '^C',
  'QUIT' => '^]',
  'TSTP' => '^Z',
}
script = STDIN.readlines.map {|l| l.chomp }
STDOUT.sync = true

begin
  PTY.spawn(*ARGV) do |pin, pout, pid|
    pin.sync = true
    pout.sync = true
    Thread.new do
      sleep 0.5
      script.each do |cmd|
        case cmd
        when /!sleep (\d+)/
          sleep $1.to_f / 2
        when /!break/
          pout.puts ''
        when /!kill (\w+)/
          sig = $1
          print sig2esc[sig]
          Process.kill(sig, pid)
        when /!eof/
          puts '^D'
          pout.close
        else
          pout.puts cmd
        end
      end
      sleep 0.5
      Process.kill("KILL", pid) rescue nil
    end

    loop do
      print(pin.sysread(512)) rescue break
    end
  end
rescue EOFError
end
