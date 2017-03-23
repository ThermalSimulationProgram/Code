# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import Config  




config = Config()
# create a new sub directory
config.set_xml_csv_sub_dir('adaption_period_event1/')

config.set_simulation_duration(45)
config.set_relative_deadline(75)
base_value = 10
control = [1, 1, 0]
for x in range(0, 10):
	new_value = base_value + x*2
	# change the value
	config.set_kernel_period(new_value)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('adaption_period' + str(new_value))
	config.run_all_kernels(control)