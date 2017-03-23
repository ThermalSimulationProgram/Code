# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *




config = Config()
heat_cpu(config)
# create a new sub directory
config.set_xml_csv_sub_dir('relative_deadline_event2/')

config.set_simulation_duration(100)
base_value = 700
control = [1, 1, 0]

for x in range(0, 15):
	new_value = base_value + x * 20
	# change the value
	config.set_relative_deadline(new_value)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('deadline' + str(new_value))
	config.run_all_kernels(control)








