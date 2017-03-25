# -*- coding=utf-8 -*-  
# python2.7
import os
from Config import *





all_benchmarks = ["ackermann",	
					"bitops",
					"correlate",
					"crc16",	
					"djb2a",	
					"euler",	
					"explog",		
					"fft",		
					"fibonacci",	
					"float", 		
					"fnv1a",		
					"gcd",		
					"hamming",		
					"hyperbolic",
					"idct",		
					"int", 		
					"jenkin",		
					"jmp",		
					"ln2",		
					"loop",		
					"matrixprod",
					"nsqrt",		
					"omega",		
					"phi",		
					"pi",			
					"pjw",		
					"prime",		
					"psi",		
					"queens",		
					"sdbm",		
					"sqrt", 		
					"trig",		
					"union", 'default'];


config = Config()
heat_cpu(config)
# create a new sub directory
config.set_xml_csv_sub_dir('benchmark1/')

config.set_simulation_duration(100)

control = [1, 1, 0]



for benchmark in all_benchmarks:
	# change the value
	config.set_benchmark_name(benchmark)
	#(b, T, allt) = get_best_bfactor(config)
	# print b 
	# print T
	# print allt
	# config.set_b_factor(b)
	# give the prefix for the files
	config.set_xml_csv_file_prefix('benchmark_name_' + benchmark)
	config.run_all_kernels(control)








