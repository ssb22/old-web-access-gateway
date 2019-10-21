#!/bin/bash
git pull --no-edit
wget -N http://ssb22.user.srcf.net/access/access.tar.gz
tar -zxvf access.tar.gz
tar -zxvf imgserver.tar.gz
git add *.c++ *.h *.tbl COPYING *.dat *.sh *.htm
git commit -am update && git push
