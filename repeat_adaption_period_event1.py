# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *

import time

import os
from Config import *

import time

os.system('sudo cpupower -c all frequency-set -g performance');

config = Config()
# heat_cpu(config)
# create a new sub directory
config.set_xml_csv_sub_dir('adaption_period_event5/')

config.set_simulation_duration(100)
config.set_relative_deadline(1000)

ton = [152  ,100,   46 ,   67];
toff = [0.4980,   19.7020,   20.1000 ,  10.1000];

config.set_kernel_ton(ton);
config.set_kernel_toff(toff);

base_value = 50
# config.is_save_result = False
control = [1, 1, 0]
for x in range(0, 1):
	new_value = base_value + x*30
	# change the value
	config.set_kernel_period(new_value)
	# (b, T, allt) = get_best_bfactor(config)
	config.set_b_factor(0.94)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('adaption_period' + str(new_value))
	config.run_all_kernels(control, 0)



cwp = os.getcwd();
path = cwp + '/result/xml/adaption_period_event5/';

heatup = 'sudo ./main -a 1 ./result/xml/adaption_period_event5/adaption_period50_bws.xml'

heatup2 = 'sudo ./main -a 1 ./result/xml/adaption_period_event5/adaption_period50_aptm.xml'

for i in range(0, 80):
	os.system(heatup);
for i in range(0, 80):
 	os.system(heatup2);

# os.system('reboot')


base_value = 200
kernels = ['aptm', 'bws']


for y in range (0, 0):
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
	


	



