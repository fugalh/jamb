# given a stops directory
# generate a Control interface for each instrument (stops/*/definition).
# http://charlie-roberts.com/Control/

# Aeolus instrument definitions have 
# Keyboards, Divisions, Interface Groups
# The primary concern is the Interface Groups

# Aeolus accepts MIDI control messages on controller #98. Summary of things
# that can be done (see Aeolus README section 6):
# . group select+reset (01mm0ggg where mm = 00)
# . group select+set on/off/toggle mode (01mm0ggg for mm = 01/10/11)
# . button select (nothing/on/off/toggle depending on group mode) (000bbbbb)

# Stuff in the definition file:
# /divis/new $label $keyboard $section
# /rank $pan $delay $filename
# /swell (make a slider)
#
# /group/new $label
# /stop $keyboard $division $rank (xref with /rank for filename, and extract the label from there)
# /tremul $division $mnemonic $label
# /coupler $keyboard $division $mnemonic $label
# 
# In addition
# . instrument
# - MIDICTL_BANK (which bank)
# - program change (which preset)
# - MIDICTL_HOLD 
# - MIDICTL_ASOFF (All sound off) 
# - MIDICTL_ANOFF (All notes off)
#
# . division
# - swell
# - tremul frequency
# - tremul amplitude
#
# MIDI messages that are understood are code-documented in model.cc
# Model::proc_qmidi(). See also global.h
#
# So, interface groups are not necessarily the same as divisions, but I think
# as long as the names are the same we could get away with grouping a swell
# slider with that interface group. The "combination action" bits should also
# be on the main screen. The rest can be on a separate page (if anywhere).
# General cancel should exist of course.

# combination action
# http://www.organ.byu.edu/orpipe.html
# S for set
# 0 for general cancel
# M1-32 for bank (other page)
# 1-32 for presets. (all 32 on other page, just what's convenient on playing page)
#
# aeolus only has general presets, but it has multiple banks. of course if we
# feel inclined we could store preset info locally and do full combination
# action (or, hack aeolus to have per-division presets) 

# layout:
# basic unit is roughly 1em. Stop buttons are 4x2. There's a 1 unit gap between
# divisions (the division name goes in here too). The combination action at the
# bottom is 1 unit tall, and its buttons are 1x1.

require 'ostruct'
class AeolusInstrument
  def initialize(instrument_dir)
    @label = File.basename(instrument_dir)
    @keyboards = []
    @divisions = []
    @groups = []

    d = nil
    g = nil
    gi = 1
    bi = 1
    File.readlines(instrument_dir + '/definition').each do |line|
      tokens = line.strip.split
      case tokens[0]
      when '/manual/new', '/pedal/new'
        @keyboards.push tokens[1]
      when '/divis/new'
        d = OpenStruct.new
        d.label = tokens[1]
        d.ranks = []
        d.swell = false
        @divisions.push d
      when '/divis/end'
        d = nil
      when '/rank'
        rank = OpenStruct.new([:pan, :delay, :filename].
                              zip(tokens[1..3]))
        rank.filename = instrument_dir + '/../' + rank.filename
        # the label is the 32 characters at offset 32
        File.open(rank.filename) do |f|
          f.seek(32)
          rank.label = f.read(32).unpack("Z*")[0].sub('$', "\n")
          f.seek(32+32+56)
          rank.mnemonic = f.read(8).unpack("Z*")[0]
        end
        d.ranks.push rank
      when '/swell'
        d.swell = true
      when '/group/new'
        g = OpenStruct.new
        g.label = tokens[1]
        g.buttons = []
        g.index = gi
        gi += 1
        bi = 1
        @groups.push g
      when '/group/end'
        g = nil
      when '/stop'
        b = OpenStruct.new
        b.type = :stop
        b.keyboard = @keyboards[tokens[1].to_i - 1]
        b.division = @divisions[tokens[2].to_i - 1]
        b.rank = b.division.ranks[tokens[3].to_i - 1]
        b.label = b.rank
        b.index = bi
        bi += 1
        g.buttons.push b
      when '/coupler'
        b = OpenStruct.new
        b.type = :coupler
        b.keyboard = @keyboards[tokens[1].to_i - 1]
        b.division = @divisions[tokens[2].to_i - 1]
        b.mnemonic = tokens[3]
        b.label = tokens[4]
        b.index = bi
        bi += 1
        g.buttons.push b
      when '/tremul'
        if g
          b = OpenStruct.new([:mnemonic, :label].
                             zip(tokens[2..3]))
          b.type = :tremul
          b.division = @divisions[tokens[1].to_i - 1]
          b.index = bi
          bi += 1
          g.buttons.push b
        end
      when '/instr/end'
        break
      end
    end
  end
end
require 'yaml'
puts AeolusInstrument.new('stops/Aeolus').to_yaml
