#!/usr/bin/python
import sys
import re

if (len(sys.argv) < 2):
  print "USAGE: %s [ FILE ]" % (sys.argv[0])
  sys.exit(1)

try:
  file = open(sys.argv[1],'r')
except IOError as (errno, strerror):
  print strerror
  sys.exit(1)

errors=[]
warnings=[]
comment=False

def comment_check(line, linenumber, array):
  if (bool(re.match("//",line.strip()))):
    array.append({"line":linenumber
                 ,"msg":"You shouldn't be using // comments"
                 ,"hint":"Change them over to C-style comments"})

def tabs_check(line, linenumber, array):
  if (bool(re.match("^\t",line))):
    array.append({"line":linenumber
                 ,"msg":"Please use tabs as indentation"
                 ,"hint":"Change spaces to tabs"})

counter = 1;
for i in file.readlines():
  if ("/*" in i and "*/" not in i):
    comment=True
  comment_check(i, counter, warnings)
  if (not comment):
    tabs_check(i, counter, warnings)
  if (comment and "*/" in i):
    comment=False
  counter += 1

file.close()
for i in warnings:
  print "Line: %4s ~ %s" % (i['line'], i['msg'])
for i in errors:
  print "Line: %4s ~ %s" % (i['line'], i['msg'])
if (len(warnings) > 0):
  print "There were a total of %s warnings." % (len(warnings))
if (len(errors) > 0):
  print "There were a total of %s errors." % (len(errors))
if (len(errors) == 0 and len(warnings) == 0):
  print "No issues found, well done."
