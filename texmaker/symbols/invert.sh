#!/bin/bash
for img in *.png
 do 
  convert "$img" -negate "inverted_$img"
 done 
