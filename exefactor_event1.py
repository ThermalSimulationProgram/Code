# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *





config = Config()
heat_cpu(config)
# create a new sub directory
config.set_xml_csv_sub_dir('exefactor_event2/')

config.set_simulation_duration(80)

deadline = 720
config.set_relative_deadline(deadline)

prefix = './MATLAB_code/ARM4Test2_deadlinefactor_'
solution = readcsv(prefix + str(deadline) + '.csv');
toff = solution[0:4]
ton  = solution[4:8]

	
config.set_kernel_ton(ton)
config.set_kernel_toff(toff)

base_value = 0.1
control = [0, 0, 1]
for x in range(0, 3):
	new_value = base_value + x*0.3
	# change the value
	config.set_exe_factor(new_value)
	# (b, T, allt) = get_best_bfactor(config, 2)
	# config.set_b_factor(b)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('exefactor' + str(new_value))
	config.run_all_kernels(control, 60)








