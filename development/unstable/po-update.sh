#!/bin/sh
(cd ../../po/ && msgcat --lang=fr --use-first fr.po fr.po.staging -o fr.po)
(cd ../../qt5-dev/po/ && make update-po)
