# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *

import time


config = Config()

# create a new sub directory
config.set_xml_csv_sub_dir('temperature_profile/')

config.set_simulation_duration(100)
config.set_relative_deadline(1000)

ton = [50, 50,  50,   50];
config.set_kernel_ton(ton);

base_value = 0
# config.is_save_result = False
control = [0, 0, 1]
for x in range(0, 50):	
	new_value = base_value + x*2
	# change the value
	toff = [new_value, new_value, new_value, new_value]
	config.set_kernel_toff(toff);
	# give the prefix for the files
	config.set_xml_csv_file_prefix('toff' + str(new_value))
	config.run_all_kernels(control, 100)


ton = [50, 50,  50,  0];
config.set_kernel_ton(ton);
# create a new sub directory
config.set_xml_csv_sub_dir('temperature_profile2/')
base_value = 0
# config.is_save_result = False
control = [0, 0, 1]
for x in range(0, 50):	
	new_value = base_value + x*2
	# change the value
	toff = [new_value, new_value, new_value, 1000]
	config.set_kernel_toff(toff);
	# give the prefix for the files
	config.set_xml_csv_file_prefix('toff' + str(new_value))
	config.run_all_kernels(control, 100)

ton = [50, 0,  50,  0];
config.set_kernel_ton(ton);
# create a new sub directory
config.set_xml_csv_sub_dir('temperature_profile3/')
base_value = 0
# config.is_save_result = False
control = [0, 0, 1]
for x in range(0, 50):	
	new_value = base_value + x*2
	# change the value
	toff = [new_value, 1000, new_value, 1000]
	config.set_kernel_toff(toff);
	# give the prefix for the files
	config.set_xml_csv_file_prefix('toff' + str(new_value))
	config.run_all_kernels(control, 100)

ton = [50, 0,  0,  0];
config.set_kernel_ton(ton);
# create a new sub directory
config.set_xml_csv_sub_dir('temperature_profile4/')
base_value = 0
# config.is_save_result = False
control = [0, 0, 1]
for x in range(0, 50):	
	new_value = base_value + x*2
	# change the value
	toff = [new_value, 1000, 1000, 1000]
	config.set_kernel_toff(toff);
	# give the prefix for the files
	config.set_xml_csv_file_prefix('toff' + str(new_value))
	config.run_all_kernels(control, 100)