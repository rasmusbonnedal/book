import csv
import pyperclip
import sys
from datetime import datetime

def addToClipboard(text):
    pyperclip.copy(text)

def parser(lines, mindate):
    csvlen = len(lines[0].split(','))
    if csvlen == 10:
        print('Detected Komplett')
        return parseKomplettBank(lines, mindate)

    semicolonlen = len(lines[0].split(';'))
    if semicolonlen == 10:
        print('Detected Avanza')
        return parseAvanza(lines, mindate)

    try:
        datetime.strptime(lines[1], "%Y-%m-%d")
        print('Detected Preem')
        return parsePreem(lines, mindate)
    except Exception as e:
        print(e)

    raise Exception('Unknown format')

def parseAvanza(lines, mindate):
    result = []
    for line in lines:
        line = line.split(';')
        date = line[0]
        if '20' in date:
            curdate = datetime.strptime(date, '%Y-%m-%d')
            if mindate and curdate < mindate:
                continue
            typ = line[2].encode('ansi').decode('utf-8')
            text = line[3].encode('ansi').decode('utf-8')
            saldo = line[6]
            valuta = line[8]
            result.append('\t'.join([date, '', typ, text, '', valuta, '', saldo]))
    result.reverse()
    return '\n'.join(result)

def parseKomplettBank(lines, mindate):
    result = []
    for line in csv.reader(lines, delimiter=','):
        date = line[0]
        if '20' in date:
            curdate = datetime.strptime(date, '%d.%m.%Y')
            date = curdate.strftime('%Y-%m-%d')
            if mindate and curdate < mindate:
                continue
            text = line[2].strip()
            saldo = line[5].replace('\u00a0','')
            saldo = saldo.replace('.', ',')
            result.append('\t'.join(['', date, saldo, '', '', text]))
    result.reverse()
    return '\n'.join(result)

def parsePreem(lines, mindate):
    result = []
    for line in zip(lines[1::3], lines[0::3], lines[2::3]):
        line = list(line)
        line[2] = line[2].replace(' ', '')
        print(line[2])
        result.append('\t'.join(line))
    return '\n'.join(result)

lines = []
mindate = None

if len(sys.argv) == 1:
    lines = [x for x in pyperclip.paste().splitlines() if len(x) > 0]
else:
    with open(sys.argv[1], 'rt') as f:
        if len(sys.argv) > 2:
            mindate = datetime.strptime(sys.argv[2], "%Y-%m-%d")
        lines = f.readlines()
result = parser(lines, mindate)
addToClipboard(result)
print('Data pasted to clipboard')
