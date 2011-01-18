import os
import tempfile
import sys
import subprocess

if len(sys.argv) != 4:
	print "Usage: manhandle.py [libwhatever.a] [prefix] [objcopy binary path]"
	sys.exit()

print "Mangling Symbols in " + sys.argv[1]
print "Prefixing with " + sys.argv[2]
print "Using objcopy " + sys.argv[3]

proc = subprocess.Popen("nm --defined-only -A -a " + sys.argv[1] + " | sed -e 's/.*\ //'", stdout=subprocess.PIPE, shell=True)
things = proc.communicate()[0].split('\n')

donethings = []
tf, tfn = tempfile.mkstemp()

for i in things:
	if not i in donethings and i != "_ZNSt15basic_stringbufIcSt11char_traitsIcESaIcEED1Ev" and i != "_ZNSt6vectorIhSaIhEED1Ev":
		donethings.append(i)
		os.write(tf, i + " " + sys.argv[2] + i)
		os.write(tf, "\n")

os.close(tf)
os.system(sys.argv[3] + " --redefine-syms " + tfn + " " + sys.argv[1])
os.remove(tfn)

