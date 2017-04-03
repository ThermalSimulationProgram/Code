# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *

import time



cwp = os.getcwd();
path = cwp + '/result/xml/adaption_period_event5/';

heatup = 'sudo ./main ./result/xml/heatup_cpu_aptm.xml'

os.system(heatup);

base_value = 470
kernels = ['bws']
for y in range (0, 1):
	for x in range(0, 6):
		new_value = base_value + x*30;
		for k in kernels:
			valid_file = 'adaption_period' + str(new_value) + '_' + k + '.xml';
			absfilepath = path + valid_file;
			command = 'sudo ./main -a 1 ' + absfilepath;
			print command
			time.sleep(60)
			os.system(command);
	


	



