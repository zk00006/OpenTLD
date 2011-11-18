#!/bin/bash
datasets="*"
gt_file="gt.txt"
evaluation_file="evaluation_all_visual.txt"
for dir in $datasets; do
    if [ -d $dir ]; then
        if [ -f "$dir/$1" ]; then
			source=$(ls $dir | grep .mpg)
        	echo "[$dir]" >> $evaluation_file	
			python evaluate_vis.py "$dir/$source" "$dir/$1" "$dir/$gt_file" >> $evaluation_file
			#mv evaluation.avi $dir
		fi
    fi
done
