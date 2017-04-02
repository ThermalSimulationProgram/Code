# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *




config = Config()
# heat_cpu(config)
# create a new sub directory
config.set_xml_csv_sub_dir('relative_deadline_event4/')

config.set_simulation_duration(100)
base_value = 700
control = [0, 0, 1]

prefix = './MATLAB_code/ARM4Test2_deadlinefactor_'


for x in range(0, 13):
	new_value = base_value + x * 20
	# change the value
	config.set_relative_deadline(new_value)
	
	# load pboo solution
	solution = readcsv(prefix + str(new_value) + '.csv');

	toff = solution[0:4]
	ton  = solution[4:8]
	# print toff
	# print ton
	
	config.set_kernel_ton(ton)
	config.set_kernel_toff(toff)
	
	#(b, T, allt) = get_best_bfactor(config)
	# print b 
	# print T
	# print allt
	# config.set_b_factor(b)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('deadline' + str(new_value))
	config.run_all_kernels(control,1)








