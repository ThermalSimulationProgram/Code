# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import Config  
from xml_api import * 
 
import time
import csv

def run_all_kernels(config):
	sleep_length = 1
	all_kernels = ['aptm', 'bws', 'pboo']
	for kernel in all_kernels:
		config.set_kernel(kernel)
		config.run()
		time.sleep(sleep_length)


def change_relative_deadline():

	config = Config()
	# create a new sub directory
	config.set_xml_csv_sub_dir('relative_deadline/')

	
	base_value = 120
	for x in range(0, 1):
		new_value = base_value + x*10
		# change the value
		config.set_relative_deadline(new_value)
		# give the prefix for the files
		config.set_xml_csv_file_prefix('deadline' + str(new_value))
		run_all_kernels(config)

def change_exe_factor():

	config = Config()
	config.set_simulation_duration(1)
	# create a new sub directory
	config.set_xml_csv_sub_dir('exe_factor/')

	
	base_value = 0.1
	for x in range(0, 10):
		new_value = base_value + x*0.1
		# change the value
		config.set_exe_factor(new_value)
		# give the prefix for the files
		config.set_xml_csv_file_prefix('exe_factor' + str(new_value))
		run_all_kernels(config)

def readcsv(name):
	tempdata = []
	with open(name, 'rb') as csvfile:
		r = csv.reader(csvfile, delimiter = ',')
		for row in r:
			for d in row:
				tempdata.append(float(d))
	return tempdata

def get_best_bfactor(config):
	config.set_xml_csv_sub_dir('bfactor_temp/')
	config.set_simulation_duration(20)
	T = []
	bestT = 200
	bestb = 0.9
	for b in range(50, 98, 2):
		config.set_b_factor(b*0.01)
		config.set_xml_csv_file_prefix('bfactor' + str(b))
		config.set_kernel('aptm')
		config.run()
		time.sleep(5)
		csvfile_name = config.get_csv_filepath() + '_result.csv'
		tempdata = readcsv(csvfile_name)
		thisT = tempdata[5]
		T.append(thisT)
		if (thisT < bestT):
			bestT = thisT
			bestb = b

	return (bestb, bestT, T)
# def change_exe_factor(config):
# 	curr_dir = 'exe_factor'
# 	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)

# 	new_value = base_value = 0.1
# 	for x in range(0,10):
# 		new_value = base_value+x*0.1
# 		change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

# def change_kernel_period(config):

# 	curr_dir = 'period'
# 	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)

# 	new_value = base_value = 10
# 	for x in range(0,10):
# 		new_value = base_value+ x*10
# 		change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)
# def change_kernel_factor(config):
	
# 	curr_dir = 'b_factor'
# 	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir) 

# 	new_value = base_value = 0.6
# 	for x in range(61,97):
# 		new_value = x*0.01
# 		change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

# def change_kernel_ton(config,new_value):
# 	curr_dir = 'ton'

# 	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)
# 	change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

# def change_kernel_toff(config,new_value):
# 	curr_dir = 'toff'

# 	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)
# 	change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

# def change_event_each_arg(config,arg,new_value,eventname):

# 	kernel_type = config.get_kernel_type()
# 	read_filepath = config.get_read_file_path()
# 	read_filename = "".join([read_filepath,'/event/example_',kernel_type, '.xml'])
# 	write_filename = read_filename
# 	if arg == 'period':
# 		base_value = config.get_event_period()
# 	elif arg == 'jitter':
# 		base_value = config.get_event_jitter()
# 	elif arg == 'distance':
# 		base_value = config.get_event_distance()
# 	elif arg == 'wcets':
# 		base_value = config.get_event_wcets()
# 	else:
# 		print('init event ... ')
# 		read_filename = "".join(['example_',kernel_type, '.xml'])
# 		# write_filename = "".join([config.get_read_file_path,'/event/',read_filename])
# 		tree = read_xml(read_filename) 
# 		write_xml(tree,write_filename)
# 		return True

# 	tree = read_xml(read_filename)  
# 	nodes = find_nodes(tree,"events/"+arg)
# 	result_nodes = get_node_by_keyvalue(nodes, {"value":base_value})  
# 	change_node_properties(result_nodes, {"value": str( new_value)})
# 	write_xml(tree,write_filename)
# 	write_xml(tree,"".join([read_filepath,'/event/',eventname,'.xml']))
# def change_event_each_type(config,eventname,event_period,event_jitter,event_distance,event_wcets):
	
# 	change_event_each_arg(config,"","","")
# 	change_event_each_arg(config,"period",event_period,eventname)
# 	change_event_each_arg(config,"jitter",event_jitter,eventname)
# 	change_event_each_arg(config,"distance",event_distance,eventname)
# 	change_event_each_arg(config,"wcets",event_wcets,eventname)
	

# def change_event(config,eventname,event_period,event_jitter,event_distance,event_wcets):

# 	change_event_each_type(config,eventname,event_period,event_jitter,event_distance,event_wcets)
	
# def test_different_relative_deadline(config_aptm,config_bws,config_pboo):

# 	change_relative_deadline(config_aptm)
# 	change_relative_deadline(config_bws)
# 	change_relative_deadline(config_pboo)	

# def test_different_exe_factor(config_aptm,config_bws,config_pboo):
# 	change_exe_factor(config_aptm)
# 	change_exe_factor(config_bws)
# 	change_exe_factor(config_pboo)



if __name__ == "__main__": 
	#change_relative_deadline()
	#change_exe_factor()
	config = Config()
	(b, t, T) = get_best_bfactor(config)
	print b
	print t
	print T



