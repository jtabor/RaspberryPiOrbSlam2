#! /usr/bin/python
import sys

if (len(sys.argv) != 2):
    print "usage: splitFiles.py [filename to split]"

filename = sys.argv[1];

f = open(filename);

data = f.readlines();
f.close();

curFilename = "";
firstTime = True;

curFile = open(filename);

for i in range(0,len(data)):
    line = data[i];
    if ("FILE" in line):
        folderAndFile = line.replace("FILE: rgb/","");
        
        curFilename = folderAndFile.replace("png","txt");
        curFilename = curFilename.strip();        
        if(not firstTime):
            curFile.close();
            curFile = open(curFilename,'w');
        else:
            curFile = open(curFilename,'w');
        
    if ("ORB Feature: " in line):
        lineToWrite = line.replace("ORB Feature: ","");
        curFile.write(lineToWrite);

curFile.close();

