=begin
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

    darm_dir = File.dirname(__FILE__)
    lib_name = FFI.map_library_name('darm')
    ffi_lib File.join(darm_dir, lib_name)

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
             :rotate, :int32,
             :Rd, :int32,
             :Rn, :int32,
             :Rm, :int32,
             :Ra, :int32,
             :Rt, :int32,
             :Rt2, :int32,
             :RdHi, :int32,
             :RdLo, :int32,
             :imm, :uint32,
             :type_, :int32,
             :Rs, :int32,
             :shift, :uint32,
             :lsb, :uint32,
             :width, :uint32,
             :reglist, :uint16,
             :coproc, :uint8,
             :opc1, :uint8,
             :opc2, :uint8,
             :CRd, :int32,
             :CRn, :int32,
             :CRm, :int32,
             :firstcond, :int32,
             :mask, :uint8
    end

    class DarmStr < FFI::Struct
      layout :mnemonic, [:char, 12],
             :arg0, [:char, 32],
             :arg1, [:char, 32],
             :arg2, [:char, 32],
             :arg3, [:char, 32],
             :arg4, [:char, 32],
             :arg5, [:char, 32],
             :shift, [:char, 12],
             :total, [:char, 64]
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

  attr_reader :dinst, :instr, :instr_type, :cond

  def initialize
    @dinst = DarmInst.new
  end

  # TODO: use a wrapper function for all disasm_*
  # logic to identify instruction type and call appropriate function
  # will be implemented natively in darm 

  def disasm_armv7(blob)
    darm_armv7_disasm( @dinst, blob )
    details()
  end

  def disasm_thumb(blob)
    darm_thumb_disasm( @dinst, blob )
    details()
  end

  # not yet implemented
  def disasm_thumb2(blob)
    darm_thumb2_disasm( @dinst, blob )
    details()
  end

  def details
    @instr = "I_" + darm_mnemonic_name( @dinst[:instr] )
    @instr_type = "T_" + darm_enctype_name( @dinst[:instr_type] )
    @cond = "C_" + darm_condition_name( @dinst[:cond], 0 )
  end

  def register_list
    buf = FFI::MemoryPointer.new(32)
    darm_reglist(@dinst[:reglist], buf)
    buf.null? ? nil : buf.read_string
  end

  def shift_type
    return if darm_shift_type_name( @dinst[:type_] ).nil?
    type_name = "S_" + darm_shift_type_name( @dinst[:type_] )

    if @dinst[:Rs] >= 0
      printf "type_=%s, Rs=%s", type_name, @dinst[:Rs]
    else
      printf "type_=%s, shift=%d", type_name, @dinst[:shift]
    end
  end

  def to_s
    dstr = DarmStr.new
    darm_str2(@dinst, dstr, true)
    dstr[:total].to_str
  end
end
