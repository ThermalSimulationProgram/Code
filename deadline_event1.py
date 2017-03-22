# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import Config  




config = Config()
# create a new sub directory
config.set_xml_csv_sub_dir('relative_deadline/')


base_value = 70
for x in range(0, 11):
	new_value = base_value + x*5
	# change the value
	config.set_relative_deadline(new_value)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('deadline' + str(new_value))
	run_all_kernels(config)








