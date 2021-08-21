require 'rubygems'
require 'json'

class Random
  class << self
    @@seq = 0
    
    def random_float_range(start, finish)
      width = finish - start
      return (rand * width) + start
    end
    
    def point(x_range = -1..1, y_range = -1..1)
      x_range = Random.array(x_range) if x_range.is_a? Array
      y_range = Random.array(y_range) if y_range.is_a? Array
      
      Point.new(int_range(x_range.first, x_range.last), int_range(y_range.first, y_range.last))
    end
    
    def int_range(min, max)
      rand((max - min)) + min
    end
    
    def array(ary)
      ary[int_range(0, ary.length)]
    end
    
    def sequence(base)
      time = Random.int_range(0, base*2)
      @@seq += time
      @@seq
    end
  end
end

class Point < Struct.new(:x, :y)
  def to_json(*args)
    { "x" => self[:x], "y" => self[:y] }.to_json
  end
end

class Sprite < Struct.new(:type, :spawn_interval, :number_to_spawn, :start_spawn_time, :velocity, :start_position, :spawn_vector, :frame_delay)
  def self.random
    sprite = self.new
    sprite.type = Random.array(%w|red-blob blue-blob purple-blob orange-blob green-blob black-blob nanobot2 nanobot1 enemy|)
    sprite.spawn_interval = Random.int_range(1500, 6000)
    sprite.number_to_spawn = Random.int_range(1, 5)
    sprite.number_to_spawn = Random.int_range(1, 3) if sprite.type == "nanobot1"
    sprite.start_spawn_time = Random.sequence(1000)
    sprite.start_position = Random.point(Range.new(-100, 900), [Range.new(-99, -101), Range.new(699, 701)])

    delta_y = sprite.start_position.y < 0 ? 5..30 : -5..-30
    delta_x = sprite.start_position.x < 0 ? 5..30 : -5..-30
    
    sprite.velocity = Random.point(delta_x, delta_y)
    sprite.velocity = Point.new(20, 20) if sprite.type == "nanobot1"
    sprite.spawn_vector = Random.point(1..10, 1..10)
    sprite.frame_delay = Random.int_range(100, 700)
    sprite
  end
  
  def self.random_player
    sprite = self.new
    sprite.type = "player"
    sprite.spawn_interval = 0
    sprite.number_to_spawn = 1
    sprite.start_spawn_time = 0
    sprite.start_position = Point.new(400,300)
    sprite.velocity = Point.new(0,0)
    sprite.spawn_vector = 0
    sprite.frame_delay = 300
    sprite
  end
  
  def self.random_speed_powerup
    sprite = self.new
    sprite.type = "speed-powerup"
    sprite.spawn_interval = 30 * 1000
    sprite.number_to_spawn = 10000
    sprite.start_spawn_time = 10 * 1000
    sprite.start_position = Random.point([100..200, 500..600], [-100..-101, 700..701])
    m = sprite.start_position.y < 0 ? 1 : -1
    sprite.velocity = Point.new(0, m * 25)
    sprite.spawn_vector = Random.point(1..10, 1..10)
    sprite.frame_delay = 400
    sprite
  end
  
  def to_hsh
    datas = {}
    self.each_pair do |symbol, value|
      datas[symbol.to_s.gsub("_", " ")] = value
    end
    datas
  end
end

class Level < Struct.new(:name)
  attr_reader :sprites
  def initialize(name)
    self[:name] = name
    @sprites = []
    @name_count = 0
  end
  
  def random_name
    @name_count += 1
    "name_#{@name_count}"
  end
  
  def to_hsh
    sprites = {}
    @sprites.collect { |s| s.to_hsh }.each do |s|
      sprites[random_name] = s
    end
    {
      "name" => self.name,
      "proto sprites" => sprites
    }
  end
  
  def add_sprite(sprite)
    @sprites << sprite
  end
  
  def self.generate_name
    "Random level #{Time.now}"
  end
  
  def self.random(number_sprites)
    level = self.new("Random level #{Time.now}")
    number_sprites.times do
      level.add_sprite(Sprite.random)
    end
    level.add_sprite(Sprite.random_player)
    level.add_sprite(Sprite.random_speed_powerup)
    level
  end
end

puts Level.random(ARGV[0].to_i).to_hsh.to_json