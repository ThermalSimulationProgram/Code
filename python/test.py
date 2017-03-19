# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import Config  
from xml_api import * 
  



def change_filename(config,tree,new_filepath,new_filename):
	# print(new_filename)
	base_path = config.get_write_file_path()
	new_filename = ''.join([base_path,'/',new_filename])
	tree_root = tree.getroot()
	# print tree_root.attrib
	tree_root.set('name',new_filename)
 	
 # 	nodes = find_nodes(tree, "scheduler/kernel") 
	# result_nodes = get_node_by_keyvalue(nodes, {"type":"APTM"}) 
	# change_node_properties(result_nodes, {"type": kernel_type})
	# print(new_filepath)
	write_xml(tree,"".join([new_filepath,'.xml']))

def get_abs_path_and_filename(config,curr_dir):
	
	kernel_type     = config.get_kernel_type()	
	xml_path        = config.get_read_file_path()
	read_filepath   = ''.join([config.get_read_file_path(),'/','event/'])
	read_filename   = ''.join([read_filepath,'example_',kernel_type.lower(),'.xml'])
	base_filename   = ''.join([curr_dir,'_',kernel_type,'_'])
	write_xml_path  = ''.join([xml_path,'/',curr_dir,'/'])
	return read_filename,base_filename,write_xml_path

def exe_command(command):
	command_exe = 'sudo ./main '+command
	print command_exe
	if not no_command:
		os.system(command_exe)

def change_init(config,curr_dir):
	''' base arg :filename ,path and ...'''

	read_filename,base_filename,xml_path = get_abs_path_and_filename(config,curr_dir)

	''' xml tree'''
	tree = read_xml(read_filename)  
	if curr_dir == 'relative_deadline':
		base_value = config.get_relative_deadline()
		nodes = find_nodes(tree, "".join(["events/",curr_dir])) 
	elif curr_dir == 'exe_factor':
		base_value = config.get_exe_factor()
		nodes = find_nodes(tree, "".join(["events/",curr_dir])) 
	elif curr_dir == 'period':
		base_value = config.get_kernel_period()
		nodes = find_nodes(tree, "".join(["scheduler/kernel/",curr_dir])) 
	elif curr_dir == 'b_factor':
		base_value = config.get_b_factor()
		nodes = find_nodes(tree, "".join(["scheduler/kernel/",curr_dir])) 
	elif curr_dir == 'ton':
		base_value = config.get_kernel_ton()
		nodes = find_nodes(tree, "".join(["scheduler/kernel/",curr_dir])) 
 	elif curr_dir == 'toff':
		base_value = config.get_kernel_toff()
		nodes = find_nodes(tree, "".join(["scheduler/kernel/",curr_dir])) 
	else:
		print('dir name error ...')
		return False
	
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_value})  
	
	return read_filename,base_filename,xml_path,tree,result_nodes
def change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes):
	''' change arg 
	    write to result dir
	    execute'''

	new_filename = base_filename+str(new_value)
	new_filepath = "".join([xml_path,new_filename])
	change_filename(config,tree,new_filepath,"".join([curr_dir, "/",new_filename]))
	change_node_properties(result_nodes, {"value": str(new_value)})  
		
	write_filename = "".join([new_filepath,'.xml'])
	write_xml(tree,write_filename)
	exe_command(write_filename)

def change_relative_deadline(config):

	curr_dir = 'relative_deadline'
	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)
	
	new_value = base_value = 120
	for x in range(0,1):
		new_value = new_value+x*10
		change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

def change_exe_factor(config):
	curr_dir = 'exe_factor'
	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)

	new_value = base_value = 0.1
	for x in range(0,10):
		new_value = base_value+x*0.1
		change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

def change_kernel_period(config):

	curr_dir = 'period'
	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)

	new_value = base_value = 10
	for x in range(0,10):
		new_value = base_value+ x*10
		change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)
def change_kernel_factor(config):
	
	curr_dir = 'b_factor'
	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir) 

	new_value = base_value = 0.6
	for x in range(61,97):
		new_value = x*0.01
		change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

def change_kernel_ton(config,new_value):
	curr_dir = 'ton'

	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)
	change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

def change_kernel_toff(config,new_value):
	curr_dir = 'toff'

	read_filename,base_filename,xml_path,tree,result_nodes=change_init(config,curr_dir)
	change_exe(config,new_value,base_filename,xml_path,curr_dir,tree,result_nodes)

def change_event_each_arg(config,arg,new_value,eventname):

	kernel_type = config.get_kernel_type()
	read_filepath = config.get_read_file_path()
	read_filename = "".join([read_filepath,'/event/example_',kernel_type, '.xml'])
	write_filename = read_filename
	if arg == 'period':
		base_value = config.get_event_period()
	elif arg == 'jitter':
		base_value = config.get_event_jitter()
	elif arg == 'distance':
		base_value = config.get_event_distance()
	elif arg == 'wcets':
		base_value = config.get_event_wcets()
	else:
		print('init event ... ')
		read_filename = "".join(['example_',kernel_type, '.xml'])
		# write_filename = "".join([config.get_read_file_path,'/event/',read_filename])
		tree = read_xml(read_filename) 
		write_xml(tree,write_filename)
		return True

	tree = read_xml(read_filename)  
	nodes = find_nodes(tree,"events/"+arg)
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_value})  
	change_node_properties(result_nodes, {"value": str( new_value)})
	write_xml(tree,write_filename)
	write_xml(tree,"".join([read_filepath,'/event/',eventname,'.xml']))
def change_event_each_type(config,eventname,event_period,event_jitter,event_distance,event_wcets):
	
	change_event_each_arg(config,"","","")
	change_event_each_arg(config,"period",event_period,eventname)
	change_event_each_arg(config,"jitter",event_jitter,eventname)
	change_event_each_arg(config,"distance",event_distance,eventname)
	change_event_each_arg(config,"wcets",event_wcets,eventname)
	

def change_event(config,eventname,event_period,event_jitter,event_distance,event_wcets):

	change_event_each_type(config,eventname,event_period,event_jitter,event_distance,event_wcets)
	
def test_different_relative_deadline(config_aptm,config_bws,config_pboo):

	change_relative_deadline(config_aptm)
	change_relative_deadline(config_bws)
	change_relative_deadline(config_pboo)	

def test_different_exe_factor(config_aptm,config_bws,config_pboo):
	change_exe_factor(config_aptm)
	change_exe_factor(config_bws)
	change_exe_factor(config_pboo)

def prepare_dir():

	xml_path = config.get_read_file_path()
	csv_path = config.get_write_file_path()
	make_dirs(xml_path)
	make_dirs(csv_path)	
	function_dirs = ['relative_deadline','exe_factor','period','b_factor','ton','toff','event']
	for function_dir in function_dirs:
		xml_dir = ''.join([xml_path,'/',function_dir])
		csv_dir = ''.join([csv_path,'/',function_dir])
		make_dirs(xml_dir)
		make_dirs(csv_dir)

if __name__ == "__main__": 
	global event 
	event = "event1"
	
	global no_command
	no_command = True
	

	config = Config('global')
	config_aptm = Config('APTM')
	config_bws = Config('BWS')
	config_pboo = Config('PBOO')
	prepare_dir()
	change_event(config_aptm, event,120,151,1,"{14.2 10 3.6 5.7}")
	change_event(config_bws, event,120,151,1,"{14.2 10 3.6 5.7}")
	change_event(config_pboo, event,120,151,1,"{14.2 10 3.6 5.7}")

	test_different_relative_deadline(config_aptm,config_bws,config_pboo)
	test_different_exe_factor(config_aptm,config_bws,config_pboo)

	change_kernel_period(config_aptm)
	change_kernel_period(config_bws)
	

	change_kernel_factor(config_aptm)
	
	change_kernel_ton(config_pboo,"{10,10,10,10}")

	change_kernel_toff(config_pboo,"{10,10,10,10}")



