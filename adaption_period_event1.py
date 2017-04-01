# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *

import time


# config = Config()
# # heat_cpu(config)
# # create a new sub directory
# config.set_xml_csv_sub_dir('adaption_period_event5/')

# config.set_simulation_duration(60)
# config.set_relative_deadline(1000)
# config.is_save_result = False
# base_value = 200
# # config.is_save_result = False
# control = [1, 0, 0]
# for x in range(0, 15):
# 	new_value = base_value + x*30
# 	# change the value
# 	config.set_kernel_period(new_value)
# 	(b, T, allt) = get_best_bfactor(config)
# 	config.set_b_factor(b)
# 	# give the prefix for the files
# 	config.set_xml_csv_file_prefix('adaption_period' + str(new_value))
# 	config.run_all_kernels(control)
# 	time.sleep(30)


cwp = os.getcwd();
path = cwp + '/result/xml/adaption_period_event5/';

base_value = 260

for x in range(0, 13):
	new_value = base_value + x*30;
	valid_file = 'adaption_period' + str(new_value) + '_aptm.xml';
	absfilepath = path + valid_file;
	command = 'sudo ./main ' + absfilepath;
	print command
	time.sleep(60)
	os.system(command);
	


	



