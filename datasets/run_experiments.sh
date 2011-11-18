#!/bin/bash
parameter_file="parameters.yml"
datasets="../datasets/*"
gt_file="gt.txt"
bb_file="init.txt"
for dir in $datasets; do
	if [ -d $dir ]; then
		echo $dir
		source=$(ls $dir | grep .mpg)
		if [ -f "$dir/initial_detector.txt" ]; then
			echo "[$dir]  Skipping initial detector" 
		else
			echo "running $source"
			./run_tld -p $parameter_file -s "$dir/$source" -b "$dir/$bb_file"
			if [ $? == 0 ]; then
				mv bounding_boxes.txt "$dir/initial_detector.txt"
			else
				echo "$dir Failed initial detector" >> fail.txt
			fi
		fi
		if [ -f "$dir/pn_tracker.txt" ]; then
			echo "[$dir] Skipping PN Tracker"
		else
			./run_tld -p $parameter_file -s "$dir/$source" -b "$dir/$bb_file" -tl -r
			if [ $? == 0 ]; then	
				mv bounding_boxes.txt "$dir/pn_tracker.txt"
				mv final_detector.txt "$dir/final_detector.txt"
			else
				echo "$dir Failed PN tracker" >> fail.txt
			fi
		fi
	fi
done
