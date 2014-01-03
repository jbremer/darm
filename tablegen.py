"""
Copyright (c) 2013, Jurriaan Bremer
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
"""

import re


class InstructionFormat(object):
    def __init__(self, fmt):
        self.fmt = fmt
        self.sm = []

        self.process()

    def process(self):
        fmt = self.fmt

        if '{S}' in fmt:
            self.sm.append('STR_S')

        if '<c>' in fmt:
            self.sm.append('STR_cond')

        args = fmt.split(' ', 1)[1].split(',') if ' ' in fmt else []

        regs = '<Rd>', '<Rn>', '<Rm>', '<Rt>', '<Rt2>', '<RdHi>', \
               '<RdLo>', '<Ra>', '<CRd>', '<CRn>', '<CRm>'

        imms = '#<const>', '#<imm>', '#<imm4>', '#<imm5>', '#<imm8>', \
               '#<imm12>', '#<imm16>', '#<imm24>'

        t = {
            '<registers>': ['STR_REGLIST'],
            '<type> <Rs>': ['STR_SHIFT'],
            '#<option>': ['STR_OPTION'],
            '#<shift>': ['STR_SHIFT'],
            '<label>': ['STR_LABEL'],
            'SP': ['STR_REG_CONST', 'SP'],
            '#<lsb>': ['STR_INT', 'O(lsb)'],
            '#<width>': ['STR_INT', 'O(width)'],
            '<Rn>{!}': ['STR_REG', 'O(Rn)', 'STR_EXCL'],
            '<coproc>': ['STR_COPROC'],
            '<shift>': ['STR_SHIFT'],
        }

        t.update(dict((_, ['STR_IMM']) for _ in imms))
        t.update(dict((_, ['STR_REG', 'O(%s)' % _[1:-1]]) for _ in regs))

        for arg in (_.strip() for _ in args):
            if arg in t:
                self.sm += t[arg]
                continue

        self.sm.append('STR_RETN')

    def create(self):
        return self.sm


class Instruction(object):
    def __init__(self, fmt, bits, **kwargs):
        self.fmt = InstructionFormat(fmt)
        self.bits = bits

        self.name = re.split(r'\W', fmt)[0].lower()

        # A mapping of bit indices to their integer value, and a mapping of
        # field names to their offset in bits and value.
        self.value, self.field, off = {}, {}, 0
        for idx, bit in enumerate(bits):
            if isinstance(bit, int):
                self.value[off] = bit
                off += 1
            else:
                self.field[bit.name] = idx, bit
                off += bit.bitsize

        # Some instructions have bit patterns which are partially hardcoded.
        self.hardcoded = {}
        for k, v in kwargs.items():
            if not k in self.field:
                continue

            kwargs.pop(k)
            self.hardcoded[k] = v

        # The remainder of the keyword arguments are macro's.
        self.macros = kwargs

    def field_index(self, name):
        assert name in self.field
        return self.field[name][0]

    def bitsize(self, off):
        """Calculates the bitsize up to offset.

        @off: Offset to calculate up to, not inclusive.
        """
        return sum(getattr(_, 'bitsize', 1) for _ in self.bits[:off])

    def __repr__(self):
        return '<Instruction %s, %r>' % (self.name, self.bits)

    def create(self, sm, lut, fmt, bitsize):
        idx, ret, last = 0, sm.offset(), []
        for bit in self.bits:
            if isinstance(bit, int):
                idx += 1
                continue

            if bit.last:
                last.append((bit, idx))
            else:
                bit.create(idx, sm, lut, fmt, bitsize)
            idx += bit.bitsize

        for bit, idx in last:
            bit.create(idx, sm, lut, fmt, bitsize)

        for macro in self.macros.values():
            macro.create(sm, lut, fmt, bitsize)

        name = 'I_' + self.name.upper()
        sm.append('SM_INSTR', 'L(%s)' % name, 'H(%s)' % name)
        buf = self.fmt.create()
        off = [_ for _ in xrange(len(fmt.table))
               if fmt.table[_:_+len(buf)] == buf]
        off = off[0] if off else fmt.append(*buf)
        sm.append('SM_STR', 'L(%d)' % off, 'H(%d)' % off)
        sm.append('SM_RETN')
        return ret


class BitPattern(object):
    def __init__(self, bitsize, name, last=False):
        self.bitsize = bitsize
        self.name = name
        self.last = last

    def __repr__(self):
        clz = self.__class__.__name__
        return '<%s %s, %d bits>' % (clz, self.name, self.bitsize)

    def create(self, idx, sm, lut, fmt, bitsize):
        raise


class NopField(BitPattern):
    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.offset()


class Field(BitPattern):
    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize)


class FieldPlus(BitPattern):
    def __init__(self, bitsize, name, value):
        BitPattern.__init__(self, bitsize, name)
        self.value = value

    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR2', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize, self.value)


class ScatteredField(BitPattern):
    def __init__(self, bitsize, name, offset):
        BitPattern.__init__(self, bitsize, name, last=True)
        self.offset = offset

    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR3', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize, self.offset)


class Register(BitPattern):
    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize)


class DoubleRegister(BitPattern):
    def __init__(self, bitsize, name, name2):
        BitPattern.__init__(self, bitsize, name)
        self.name2 = name2

    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize,
                         'SM_EXTR', 'O(%s)' % self.name2,
                         bitsize-self.bitsize-idx, self.bitsize)


class CoprocessorRegister(BitPattern):
    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR2', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize, 'CR_BASE')


class FloatingPointRegister(BitPattern):
    def __init__(self, bitsize, name, msb):
        BitPattern.__init__(self, bitsize, name)
        self.msb = msb

    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_FPREG', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.msb)


class ScatteredRegister(BitPattern):
    def __init__(self, bitsize, name, offset):
        BitPattern.__init__(self, bitsize, name, last=True)
        self.offset = offset

    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR3', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize, self.offset)


class DoubleScatteredRegister(BitPattern):
    def __init__(self, bitsize, name, name2, offset):
        BitPattern.__init__(self, bitsize, name, last=True)
        self.name2 = name2
        self.offset = offset

    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_EXTR3', 'O(%s)' % self.name,
                         bitsize-self.bitsize-idx, self.bitsize, self.offset,
                         'SM_EXTR3', 'O(%s)' % self.name2,
                         bitsize-self.bitsize-idx, self.bitsize, self.offset)


class Immediate(BitPattern):
    def create(self, idx, sm, lut, fmt, bitsize):
        # The immediate is located starting at the lowest significant bit.
        if idx + self.bitsize == bitsize:
            return sm.append('SM_IMM', self.bitsize)
        else:
            return sm.append('SM_IMM2', self.bitsize,
                             bitsize-self.bitsize-idx, 0)


class ScatteredImmediate(BitPattern):
    def __init__(self, bitsize, name, imm_idx):
        BitPattern.__init__(self, bitsize, name)
        self.imm_idx = imm_idx

    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_IMM2', self.bitsize,
                         bitsize-self.bitsize-idx, self.imm_idx)


class ScatteredSignExtendImmediate(ScatteredImmediate):
    def create(self, idx, sm, lut, fmt, bitsize):
        ret = ScatteredImmediate.create(self, idx, sm, lut, fmt, bitsize)
        sm.append('SM_SIGN', self.imm_idx)
        return ret


class BranchNotXorImmediate(ScatteredImmediate):
    def create(self, idx, sm, lut, fmt, bitsize):
        return sm.append('SM_BNXOR', bitsize-self.bitsize-idx, self.imm_idx)


class Macro(object):
    def __init__(self, name):
        self.name = name

    def __call__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        return self

    def __repr__(self):
        return '<Macro %s>' % self.name

    def create(self, sm, lut, fmt, bitsize):
        assert not self.kwargs
        return sm.append('SM_' + self.name)


class Node(object):
    def __init__(self, parent=None):
        """Initialize a new node.

        @parent: Parent node.
        """
        self.parent = parent
        self.idx = None

        # All bit indices handled by this node or its parents.
        self.indices = []
        if parent:
            self.indices += parent.indices + [parent.idx]

        self.lut = {}
        self.leaf = []
        self.hlt = None

    def insert(self, ins):
        """Insert a subnode somewhere down this node. """
        self.leaf.append(ins)

    def process(self):
        """Processes this node and creates subnodes as required."""
        # Inherit the hlt handler from our parent, if available.
        if self.parent and not self.parent.hlt is self:
            self.hlt = self.parent.hlt

        bits = dict((idx, []) for idx in xrange(32))

        for ins in self.leaf:
            for idx, bit in enumerate(ins.bits):
                if not isinstance(bit, int):
                    continue

                bit_idx = ins.bitsize(idx)
                if not bit_idx in self.indices:
                    bits[bit_idx].append(ins)

        def _sort_offsets(a, b):
            # We sort by the amount of encodings per bit index. Naturally the
            # bit index with the most associated encodings will become the
            # next node in the lookup tree.
            ret = len(bits[b]) - len(bits[a])

            # Assuming several bit indices occupy the same field, we sort by
            # the bit index, in order to make sure we always get the lowest
            # index first.
            return ret if ret else a - b

        offs = sorted(bits, cmp=_sort_offsets)
        if not offs or not bits[offs[0]]:
            assert len(self.leaf) < 2
            self.leaf = self.leaf[0] if self.leaf else None
            return

        self.idx = offs[0]

        # Currently we only support one specific instruction per set of
        # instructions.
        assert sum(not self.idx in _.value for _ in self.leaf) < 2

        def _specific_first(a, b):
            return (self.idx in a.value) - (self.idx in b.value)

        self.lut[self.idx] = Node(self), Node(self)
        for ins in sorted(self.leaf, cmp=_specific_first):
            if self.idx in ins.value:
                self.lut[self.idx][ins.value[self.idx]].insert(ins)
            else:
                self.hlt = Node(self)
                self.hlt.insert(ins)
                self.hlt.process()

        self.lut[self.idx][0].process()
        self.lut[self.idx][1].process()
        self.leaf = None

    def __repr__(self):
        if self.leaf is None:
            return '<Node %r>' % self.lut
        return '<Node %r, %r>' % (self.lut, self.leaf)

    def dump(self, idx=0):
        for bit, (null, one) in self.lut.items():
            if null.lut or null.leaf:
                print ' '*idx, '%d: 0' % bit
                null.dump(idx+1)

            if one.lut or one.leaf:
                print ' '*idx, '%d: 1' % bit
                one.dump(idx+1)

        if self.leaf:
            print ' '*idx, '->', self.leaf.name

    def create(self, sm, lut, fmt, bitsize):
        if self.leaf:
            return self.leaf.create(sm, lut, fmt, bitsize)

        if not self.lut:
            return sm.insert('SM_HLT')

        bit, (null, one) = self.lut.items()[0]

        off = sm.alloc(4)
        off2 = lut.alloc(2)

        if not null.lut and not null.leaf:
            if self.hlt:
                off_null = self.hlt.create(sm, lut, fmt, bitsize)
            else:
                off_null = sm.insert('SM_HLT')
        else:
            off_null = null.create(sm, lut, fmt, bitsize)

        if not one.lut and not one.leaf:
            if self.hlt:
                off_one = self.hlt.create(sm, lut, fmt, bitsize)
            else:
                off_one = sm.insert('SM_HLT')
        else:
            off_one = one.create(sm, lut, fmt, bitsize)

        sm.update(off, 'SM_STEP', bitsize-1-bit,
                  'L(%d)' % off2, 'H(%d)' % off2)
        lut.update(off2, off_null, off_one)
        return off


class LookupTable(object):
    def __init__(self, bits):
        self.table = []
        self.bits = bits

    def offset(self):
        return len(self.table)

    def check(self, value):
        return isinstance(value, str) or value >= 0 and value < 2**self.bits

    def alloc(self, length):
        ret = len(self.table)
        self.table += [None for _ in xrange(length)]
        return ret

    def insert(self, value):
        assert self.check(value)
        if value in self.table:
            return self.table.index(value)
        ret = len(self.table)
        self.table.append(value)
        return ret

    def update(self, offset, *args):
        assert all(self.check(_) for _ in args)
        tbl_begin = self.table[:offset]
        tbl_end = self.table[offset+len(args):]
        self.table = tbl_begin + list(args) + tbl_end

    def append(self, *args):
        assert all(self.check(_) for _ in args)
        ret = len(self.table)
        self.table += args
        return ret


class Table(object):
    def __init__(self, insns, bitsize):
        self.bitsize = bitsize
        self.insns = insns

        self.root = self._init()
        for ins in self.insns:
            if not ins.hardcoded:
                self._insert(ins)
                continue

            # When an instruction has a hardcoded field, then we pretend like
            # each combination is a unique instruction. (In order to satisfy
            # the tree, which wouldn't be able to determine the correct
            # instruction encoding otherwise.)
            assert len(ins.hardcoded) == 1
            k, v = ins.hardcoded.items()[0]
            for bits in v:
                off = ins.field_index(k)
                new_bits = list(ins.bits[:off])
                new_bits += [(bits >> _) & 1
                             for _ in xrange(ins.bits[off].bitsize)][::-1]
                new_bits += ins.bits[off+1:]
                self._insert(Instruction(ins.fmt.fmt, new_bits))

        self._process()

    def __repr__(self):
        return '<Table %r>' % self.root

    def dump(self):
        for tbl in self.root:
            tbl.dump()

    def _init(self):
        raise

    def _insert(self, ins):
        raise

    def _process(self):
        raise

    def _create(self, sm, lut, fmt, bitsize):
        raise

    def create(self):
        sm, lut, fmt = LookupTable(8), LookupTable(16), LookupTable(8)
        self._create(sm, lut, fmt, self.bitsize)
        return sm, lut, fmt
