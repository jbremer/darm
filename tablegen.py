

class Instruction(object):
    def __init__(self, name, bits, **kwargs):
        self.name = name
        self.bits = bits
        self.kwargs = kwargs

        # A mapping of bit index to its integer value.
        self.value, off = {}, 0
        for bit in bits:
            if not isinstance(bit, int):
                off += bit.bitsize
                continue

            self.value[off] = bit
            off += 1

    def bitsize(self, off):
        """Calculates the bitsize up to offset.

        @off: Offset to calculate up to, not inclusive.
        """
        return sum(getattr(_, 'bitsize', 1) for _ in self.bits[:off])

    def __repr__(self):
        return '<Instruction %s, %r>' % (self.name, self.bits)


class BitPattern(object):
    def __init__(self, bitsize, name):
        self.bitsize = bitsize
        self.name = name

    def __repr__(self):
        clz = self.__class__.__name__
        return '<%s %s, %d bits>' % (clz, self.name, self.bitsize)


class Immediate(BitPattern):
    pass


class Macro(object):
    def __init__(self, name):
        self.name = name

    def __call__(self, *args, **kwargs):
        self.args = args
        self.kwargs = kwargs
        return self

    def __repr__(self):
        return '<Macro %s>' % self.name


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

    def insert(self, ins):
        """Insert a subnode somewhere down this node. """
        self.leaf.append(ins)

    def process(self):
        """Processes this node and creates subnodes as required."""
        bits = dict((idx, []) for idx in xrange(32))

        for ins in self.leaf:
            for idx, bit in enumerate(ins.bits):
                if not isinstance(bit, int):
                    continue

                bit_idx = ins.bitsize(idx)
                if not bit_idx in self.indices:
                    bits[bit_idx].append(ins)

        def _compare(a, b):
            ret = len(bits[b]) - len(bits[a])
            return ret if ret else a - b

        offs = sorted(bits, cmp=_compare)
        if not offs or not bits[offs[0]]:
            assert len(self.leaf) < 2
            self.leaf = self.leaf[0] if self.leaf else None
            return

        self.idx = offs[0]

        self.lut[self.idx] = Node(self), Node(self)
        for ins in self.leaf:
            self.lut[self.idx][ins.value[self.idx]].insert(ins)

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


class Table(object):
    def __init__(self, insns):
        self.root = Node()

        for ins in insns:
            self.root.insert(ins)

        self.root.process()

    def __repr__(self):
        return '<Table %r>' % self.root

    def dump(self):
        self.root.dump()
