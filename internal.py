import itertools
import sys


if __name__ == '__main__':
    sys.stdout = open('darm-internal.h', 'w')

    print('#ifndef __DARM_INTERNAL__')
    print('#define __DARM_INTERNAL__')

    # define constants 0b0 up upto 0b11111111
    for x in range(256):
        print('#define %s %d' % (bin(x)[1:], x))

    # define partial constants with leading zeroes, such as 0b0001
    for x in range(2, 7):
        for y in itertools.product('01', repeat=x):
            num = ''.join(y)
            print('#define b%s %d' % (num, int(num, 2)))

    print('#endif')
