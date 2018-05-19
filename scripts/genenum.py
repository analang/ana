import sys;

def getAstPath():
  return os.path.realpath("./../src/include/ana_ast.h");


content = [x.strip(" ,") for x in open("astkind").read().split("\n")]

lines = [];

lines.append("static const char * const kind_str[] = {");

for i, item in enumerate(content):
    if i + i == len(content):
        lines.append('"%s"' % item);
    else:
        lines.append('"%s",' % item);

lines.append("};");

output = open('output.c', 'w');

for line in lines:
    if len(line) > 0:
        output.write(line + "\n");

output.close();
