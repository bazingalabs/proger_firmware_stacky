require 'serialport'
require 'ostruct'
require 'colorize'
@sp = SerialPort.new '/dev/tty.usbmodem1a1231',57600
@running = true
counter = 0
#file = File.open("VirtualSerial.hex", "r")
file = File.open("ASCIITable.cpp.hex", "rb")

Thread.abort_on_exception
t =Thread.new do
  while @running
    puts @sp.gets.yellow
  end
end

trap("INT") do
  puts "Exiting...."
  @running = false
  @sp.close
  exit
end
#(1..3).each do
#  while (line = file.gets)
#      #puts "#{counter}: #{line}"
#      @sp.write line
#      #counter = counter + 1
#      sleep 0.5
#  end
#  #end

class String
  def hex8
    [self].pack('H*').unpack('C').first
  end
  def hex16
    [self].pack('H*').unpack('S>').first
  end
end

class IHEX
  attr_reader :lines
  def initialize(s)
    @lines = []
    s.split("\n").each do |str|
      @lines << OpenStruct.new(count:str[1..2].hex8,address:str[3..6],data:str[9..-4])
    end
  end
end

puts "Start".red
@sp.puts "v"
sleep 1
@sp.puts "R"
f = file.read
ihex = IHEX.new(f).lines.each_slice(8) do |slice|
  puts "Ruby Slice".green
  puts "Ruby Address: #{slice.first.address}".green
  puts "Ruby Data: #{slice.first.data}".green
  
  @sp.puts ":80#{slice.first.address}00#{slice.collect{|s|s.data}.join}".ljust(265,"F")
  puts "#{slice.collect{|s|s.data}.join(' ')}".ljust(265,"F").green
  sleep 2
  @sp.puts "r #{slice.first.address}"
  sleep 1

end

puts "Done".green
file.close
@sp.close
puts "Done".green
#file = File.open("/var/folders/x0/t_6x4nrd4mqc7_2sft2l7d7h0000gp/T/build3476701832902264440.tmp/ASCIITable.cpp.hex", "r")