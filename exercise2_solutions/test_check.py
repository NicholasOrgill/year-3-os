#!/usr/bin/python3

#imports
import os.path
import sys
import string

#arguments
logfile=sys.argv[1]
num_clients=int(sys.argv[2])
num_lines=int(sys.argv[3])

#checks
if not os.path.isfile(logfile):
    print(logfile + " does not exist", file=sys.stderr)
    sys.exit(1)

#init
counters = [int(0) for i in range(num_clients)]
    
#iterate over file
f=open(logfile, 'r')
l=0
for i, line in enumerate(f):
    sp = line.rstrip().split(" ")
    #check correct number of strings in that line
    if len(sp) != 2:
        print(line + " line has unexpected length (should be two words in format <int> <char><int>)", file=sys.stderr)
        sys.exit(1)
    #check number in beginning of line correct
    if int(sp[0]) != i:
        print(line + " wrong line-counter (should be: {})".format(i), file=sys.stderr)
        sys.exit(1)
        
    #check counter value in client's string
    try:
        index = string.ascii_lowercase.index(sp[1][0])
    except:
        print(line + " corrupted: \"" + str(sp[1][0]) + "\" should be a single character between a-z")
        sys.exit(1)
    if counters[index] != int(sp[1][1:]):
        print(line + " wrong message (should be: {}{})".format(sp[1][0], counters[index]), file=sys.stderr)
        sys.exit(1)
    #increment
    counters[index] = counters[index] + 1
    l=i
#end for

#check lines in the file
#counting starts with 0 => clients*lines - 1
if l != ((num_lines * num_clients) - 1):
    print(logfile + " misses some lines (only {} of {})".format(l, ((num_lines * num_clients) - 1)), file=sys.stderr)
    sys.exit(1)
#end if

#check counters
for i in range(num_clients):
    if counters[i] != num_lines:
        print("client send too less/many messages (client: {} != expected: {}".format(counters[i], num_lines), file=sys.stderr)
        sys.exit(1)
    #end if
#end for

#everything fine
sys.exit(0)
