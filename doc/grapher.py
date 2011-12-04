#!/usr/bin/env python
import os
import sys
import re
# Impor graphviz
import gv

# Import pygraph
from pygraph.classes.graph import graph
from pygraph.classes.digraph import digraph
from pygraph.algorithms.searching import depth_first_search
from pygraph.readwrite.dot import write

# set a directory
dir = sys.argv[1]
# set the file extensions to look for '.m' and '.mex'
exts=['.m','.mex']
funcList = []
fileList = []
for root, subFolders, files in os.walk(dir):
   for file in files:
     fileExt = os.path.splitext(file)[-1]
     for ext in exts:
        if (fileExt == ext):
          funcList.append(os.path.splitext(file)[0])
          fileList.append(os.path.join(root,file))
#print fileList
#print funcList

# Graph creation
gr = digraph()
# Add nodes
gr.add_nodes(funcList)
#Add Edges
i=0
for file in fileList:
  f = open(file,'r')
  fbuff = f.read()
  for func in funcList:
    if (func in fbuff and func != funcList[i]):
      gr.add_edge((funcList[i],func))
  i=i+1
  fbuff = ""

# Draw as PNG
dot = write(gr)
gvv = gv.readstring(dot)
gv.layout(gvv,'dot')
gv.render(gvv,'png','dependency.png')
