import sys

def byte_to_string(b):
    s = '{ '
    last_val = len(b) - 1
    for i in range(len(b)):
        s += str(b[i]) + ', '
    s += '0 }'
    return s

print('const unsigned char cp437_to_utf8[][4] = {')
for i in range(128, 256):
    b = i.to_bytes(1);
    s = b.decode('cp437')
    u = s.encode('utf-8')
    print('    ' + byte_to_string(u) + ',')
print('};')
