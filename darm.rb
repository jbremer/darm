=begin
Copyright (c) 2013, Rohit Kumar
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* Neither the name of the darm developer(s) nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
=end

require 'ffi'

module FFI
  module Darm
    extend FFI::Library

    ffi_lib './libdarm.so'

    module Constants
      FLAGS = ['B', 'S', 'E', 'M', 'N', 'U', 'H', 'P', 'R', 'T', 'W', 'I']
      REGS = ['Rd', 'Rn', 'Rm', 'Ra', 'Rt', 'Rt2', 'RdHi', 'RdLo']
    end

    include Constants

    class DarmInst < FFI::Struct
      layout :w, :uint32,
             :instr, :uint32,
             :instr_type, :uint32,
             :cond, :uint32,
             :B, :uint32,
             :S, :uint32,
             :E, :uint32,
             :M, :uint32,
             :N, :uint32,
             :option, :int32,
             :U, :uint32,
             :H, :uint32,
             :P, :uint32,
             :R, :uint32,
             :T, :uint32,
             :W, :uint32,
             :I, :uint32,
             :rotate, :uint32,
             :Rd, :int32,
             :Rn, :int32,
             :Rm, :int32,
             :Ra, :int32,
             :Rt, :int32,
             :Rt2, :int32,
             :RdHi, :int32,
             :RdLo, :int32,
             :imm, :uint32,
             :shift_type, :int32,
             :Rs, :int32,
             :shift, :uint32,
             :lsb, :uint32,
             :width, :uint32,
             :reglist, :uint32
    end

    class DarmStr < FFI::Struct
      layout :mnemonic, [:char, 12],
             :arg0, [:char, 32],
             :arg1, [:char, 32],
             :arg2, [:char, 32],
             :arg3, [:char, 32],
             :shift, [:char, 12],
             :instr, [:char, 64]
    end

    attach_function :darm_armv7_disasm, [:pointer, :uint32], :int32
    attach_function :darm_thumb_disasm, [:pointer, :uint16], :int32
    attach_function :darm_thumb2_disasm, [:pointer, :uint16, :uint16], :int32
    attach_function :darm_mnemonic_name, [:uint32], :string
    attach_function :darm_enctype_name, [:uint32], :string
    attach_function :darm_register_name, [:int32], :string
    attach_function :darm_shift_type_name, [:int32], :string
    attach_function :darm_condition_name, [:int32, :int32], :string
    attach_function :darm_reglist, [:uint16, :pointer], :int32
    attach_function :darm_str, [:pointer, :pointer], :int32
    attach_function :darm_str2, [:pointer, :pointer, :bool], :int32
  end
end

class Darm
  include FFI::Darm

  attr_reader :d, :instr, :instr_type, :cond, :shift_type

  def initialize(blob)
    @d = DarmInst.new
    darm_armv7_disasm( @d, blob )
  end

  def disasm()
    @instr = "I_" + darm_mnemonic_name( @d[:instr] )
    @instr_type = "T_" + darm_enctype_name( @d[:instr_type] )
    @cond = "C_" + darm_condition_name( @d[:cond], 0 )
    @shift_type = darm_shift_type_name( @d[:shift_type] )
  end

  def to_s
    dstr = DarmStr.new
    darm_str2(@d, dstr, true)
    puts dstr[:instr]
  end
end
