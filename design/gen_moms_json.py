import json

def parse_markdown(file):
    with open(file, encoding='utf-8') as f:
        in_header = False
        parts = {}
        for line in f.readlines():
            if line.startswith('###'):
                if in_header:
                    parts[header] = contents
                header = line[3:].strip()
                contents = ''
                in_header = True
            elif in_header:
                contents += line
    if in_header:
        parts[header] = contents
    return parts

def expand_range(r):
    if '-' in r:
        s = [int(x) for x in r.split('-')]
        return range(s[0], s[1] + 1)
    else:
        return [int(r)]

def flatten(l):
    return [item for sublist in l for item in sublist]

def parse_parts(parts):
    jsonobj = {}
    for [k, v] in parts.items():
        subobj = {}
        post_header = False
        for line in v.splitlines():
            if post_header and line:
                content = [x.strip() for x in line.split('|') if x.strip()]
                field = content[0]
                header = content[1]
                accts = flatten([expand_range(x.strip()) for x in content[2].split(',')])
                subobj[field] = {"desc": header, "accts": accts}
            elif '----' in line:
                post_header = True
        jsonobj[k] = subobj
    return jsonobj

parts = parse_markdown('design/moms.md')
jsonobj = parse_parts(parts)
with open('design/moms_mapping.json', 'wt', encoding='utf8') as f:
    json.dump(jsonobj, f, ensure_ascii=False, indent=4)
