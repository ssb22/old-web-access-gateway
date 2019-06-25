#!/bin/bash
git pull
wget -N http://people.ds.cam.ac.uk/ssb22/access/access.tar.gz
tar -zxvf access.tar.gz
tar -zxvf imgserver.tar.gz
git add *.c++ *.h *.tbl COPYING *.dat *.sh *.htm
git commit -am update && git push
