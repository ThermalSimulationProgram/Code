# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *





config = Config()
heat_cpu(config)
# create a new sub directory
config.set_xml_csv_sub_dir('exefactor_event1/')

config.set_simulation_duration(100)
config.set_relative_deadline(720)
base_value = 0.1
control = [1, 1, 0]
for x in range(0, 10):
	new_value = base_value + x*0.1
	# change the value
	config.set_exe_factor(new_value)
	(b, T, allt) = get_best_bfactor(config)
	config.set_b_factor(b)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('exefactor' + str(new_value))
	config.run_all_kernels(control)








