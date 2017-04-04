# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *

import time



cwp = os.getcwd();
path = cwp + '/result/xml/adaption_period_event5/';

heatup = 'sudo ./main ./result/xml/heatup_cpu_aptm.xml'

os.system(heatup);

base_value = 200
kernels = ['aptm', 'bws']
for y in range (0, 10):
	valid_file = 'adaption_period' + str(900) + '_pboo.xml';
	absfilepath = path + valid_file;
	command = 'sudo ./main -a 1 ' + absfilepath;
	print command
	time.sleep(60)
	os.system(command);
	for x in range(0, 15):
		new_value = base_value + x*30;
		for k in kernels:
			valid_file = 'adaption_period' + str(new_value) + '_' + k + '.xml';
			absfilepath = path + valid_file;
			command = 'sudo ./main -a 1 ' + absfilepath;
			print command
			time.sleep(60)
			os.system(command);
	


	



