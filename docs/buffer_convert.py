# Conversion of sprites from 16-bit format
# to 8-bit format used in buffer.c
# Author: Sameer Bibikar, 28 April 2017

# Usage:
# $ python buffer_convert.py < array.c > sprites.c

import re

try:
    while True:
        s = input()
        if '{' in s or '}' in s:
            s = re.sub("unsigned short", "uint8_t", s)
            s = re.sub("uint16_t", "uint8_t", s)
            print(s)
            continue
        if not '0x' in s:
            print(s)
            continue
        print('\t', end='')
        split = re.split(", *", s)
        for nums in split:
            if not 'x' in nums:
                continue
            num = int(nums.lstrip(), base=16)
            r = int(((num & 0xF800) >> 11) * 7/31)
            g = int(((num & 0x07E0) >> 5) * 7/63)
            b = int(((num & 0x001F)) * 3/31)
            out = (r << 5) | (g << 2) | b
            print(hex(out), end='')
            print(', ', end='')
        print(); 
except EOFError:
    pass # does nothing


