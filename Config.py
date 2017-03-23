#coding:utf-8


import os
import copy
import time
import csv
  
from xml_api import *
from dir_utils import *

def readcsv(name):
	tempdata = []
	with open(name, 'rb') as csvfile:
		r = csv.reader(csvfile, delimiter = ',')
		for row in r:
			for d in row:
				tempdata.append(float(d))
	return tempdata

	
class FilePath():
	def __init__(self, root_path, sub_dir = ' ', filename = ' '):
		self.rootPath = root_path
		self.subDir = sub_dir
		self.filename = filename
		self.update_final_path()
		

	def update_final_path(self):
		self.finalPath = self.rootPath + self.subDir + self.filename

	def set_root_path(self, path):
		self.rootPath = path
		self.update_final_path()

	def set_sub_dir(self, subdir):
		self.subDir = subdir
		self.update_final_path()

	def set_file_name(self, name):
		self.filename = name
		self.update_final_path()

	def get_root_path(self):
		return self.rootPath

	def get_sub_dir_path(self):
		return self.rootPath + self.subDir

	def get_final_path(self):
		return self.finalPath

	def get_filename(self):
		return self.filename




class Config(object):
	"""docstring for config"""
	def __init__(self, xml_path='Config.xml'):
		super(Config, self).__init__()
		self.__load_xml_config(xml_path)
		self.__kernel_type = 'invalid kernel'
		self.valid_kernels = ['aptm', 'bws', 'pboo']

	def update_xml_csv_filenames(self):
		same_filename = self.__xmlfileprefix + '_' + self.__kernel_type.lower()
		self.__xml_path.set_file_name(same_filename)
		self.__csv_path.set_file_name(same_filename)

	def set_kernel(self, kernel_type):

		if kernel_type.lower() in self.valid_kernels:
			self.__kernel_type = kernel_type.lower()
			self.update_xml_csv_filenames()
		else:
			print "Illegal kernel type input!"

	'''***********************************************'''

	def get_best_bfactor(self):
		self.set_xml_csv_sub_dir('bfactor_temp/')
		self.set_kernel('aptm')
		self.set_simulation_duration(20)
		T = []
		bestT = 200
		bestb = 0.9
		for b in range(50, 98, 1):
			self.set_b_factor(b*0.01)
			self.set_xml_csv_file_prefix('bfactor' + str(b))			
			self.run()
			time.sleep(5)
			csvfile_name = self.get_csv_filepath() + '_result.csv'
			tempdata = readcsv(csvfile_name)
			thisT = tempdata[4]
			T.append(thisT)
			if (thisT < bestT):
				bestT = thisT
				bestb = b

		return (bestb, bestT, T)

	
	def run_all_kernels(self, control = [1, 1, 1]):
		sleeplength = 60
		index = 0
		for kernel in self.valid_kernels:
			if control[index] > 0:
				index = index + 1
				self.set_kernel(kernel)
				self.run()
				time.sleep(sleeplength)


	def save_to_xml(self):

		if not self.__kernel_type in self.valid_kernels:
			print "kernel type has not been set yet!"
			return

		make_dir(self.__xml_path.get_root_path())
		make_dir(self.__xml_path.get_sub_dir_path())
		print self.__xml_path.get_sub_dir_path()
		make_dir(self.__csv_path.get_root_path())
		make_dir(self.__csv_path.get_sub_dir_path())

		tree = ElementTree()
		simulation = create_node('simulation', { 'name': self.__csv_path.get_final_path() }, "")

		duration = create_time_node('duration', self.__duration_value, self.__duration_unit);
		pipeline = create_node('pipeline', {'stagenumber':str(self.__stage_number)}, "")
		simulation.append(duration)
		simulation.append(pipeline)

		events = create_node('events', {}, "")
		csv_path = create_time_node('csv_path', self.__event_csv_path, self.__event_csv_path_unit)
		period = create_time_node('period', self.__event_period_value, self.__event_period_unit)
		distance = create_time_node('distance', self.__event_distance_value, self.__event_distance_unit)
		jitter = create_time_node('jitter', self.__event_jitter_value,self. __event_jitter_unit)
		wcets = create_time_node('wcets', self.__event_wcets_value, self.__event_wcets_unit)
		deadline = create_time_node('relative_deadline', self.__relative_deadline_value, self.__relative_deadline_unit)
		exe_factor = create_node('exe_factor', {'value':str(self.__exe_factor)}, "")
		events.append(csv_path)
		events.append(period)
		events.append(jitter)
		events.append(distance)
		events.append(wcets)
		events.append(deadline)
		events.append(exe_factor)
		simulation.append(events)

		scheduler = create_node('scheduler', {}, "")
		kernel = create_node('kernel', {'type':self.__kernel_type.upper()}, "")
		if ((self.__kernel_type == 'pboo') or (self.__kernel_type == 'ge')):
			ton = create_time_node('ton', self.__kernel_ton_value, self.__kernel_ton_unit)
			toff = create_time_node('toff', self.__kernel_toff_value, self.__kernel_toff_unit)
			kernel.append(ton)
			kernel.append(toff)
		else:
			kernel_period = create_time_node('period', self.__kernel_period_value, self.__kernel_period_unit)
			kernel.append(kernel_period)
			if self.__kernel_type == 'aptm':
				bfactor = create_node('b_factor', {'value':str(self.__kernel_b_factor)},"")
				offlinedata = create_node('offlinedata', {}, "")
				prefix = create_node('prefix', {'path':self.__kernel_offline_prefix}, "")
				offlinedata.append(prefix)

				kernel.append(bfactor)
				kernel.append(offlinedata)

		scheduler.append(kernel)
		simulation.append(scheduler)

		tree._setroot(simulation)

		saved_xml_name = self.__xml_path.get_final_path() + '.xml'
		pretty_write_xml(tree, saved_xml_name)

	def run(self, flag = False):
		self.save_to_xml()
		main_to_xml_path = './' + self.__xml_path.get_final_path()
		exe_command(main_to_xml_path, flag)
		

	def __load_xml_config(self, xml_path = 'Config.xml'):
		tree = ElementTree()
		tree.parse(xml_path)

		## xml file saving path setting, csv result saving path setting
		nodes = find_nodes(tree,"path/xml_root_path")
		xmlpath = nodes[0].attrib['value']
		self.__xml_path = FilePath(xmlpath)

		nodes = find_nodes(tree,"path/csv_root_path")
		csvpath = nodes[0].attrib['value']
		self.__csv_path = FilePath(csvpath)
		
		
		self.__xmlfilename = ''
		self.__xmlfileprefix = ''

		self.__csvfilename = ''
		self.__csvfileprefix = ''


		
		
		## simulation platform setting
		# simulation name
		nodes = find_nodes(tree, "simulation")
		self.__name  = nodes[0].attrib['name']

		# simulation duration
		nodes = find_nodes(tree,"simulation/duration")
		(self.__duration_value, self.__duration_unit) = parseTimeNode(nodes[0])

		# pipeline stage number
		nodes = find_nodes(tree,"simulation/pipeline")
		self.__stage_number = nodes[0].attrib['stagenumber']


		## event setting
		nodes = find_nodes(tree, "events/csv_path")
		(self.__event_csv_path, self.__event_csv_path_unit) = parseTimeNode(nodes[0])

		# deadline
		nodes = find_nodes(tree, "events/relativeDeadline")
		(self.__relative_deadline_value, self.__relative_deadline_unit) = parseTimeNode(nodes[0])

		# exefactor
		nodes = find_nodes(tree,"events/exeFactor")
		self.__exe_factor = nodes[0].attrib['value']

		# period
		nodes = find_nodes(tree,"events/eventPeriod")
		(self.__event_period_value, self.__event_period_unit) = parseTimeNode(nodes[0])
		
		#jitter
		nodes = find_nodes(tree,"events/eventJitter")
		(self.__event_jitter_value, self.__event_jitter_unit) = parseTimeNode(nodes[0])

		# minimal interval distance
		nodes = find_nodes(tree,"events/eventDistance")
		(self.__event_distance_value, self.__event_distance_unit) = parseTimeNode(nodes[0])

		# wcets
		nodes = find_nodes(tree,"events/eventWcets")
		(self.__event_wcets_value, self.__event_wcets_unit) = parseTimeNode(nodes[0])

		## load default arguments for all types of kernels
		# adaption period, for APTM and BWS
		nodes = find_nodes(tree,"all_kernel_args/kernelPeriod")
		(self.__kernel_period_value, self.__kernel_period_unit) = parseTimeNode(nodes[0])

		# static ton, for PBOO 
		nodes = find_nodes(tree,"all_kernel_args/ton")
		(self.__kernel_ton_value, self.__kernel_ton_unit) = parseTimeNode(nodes[0])

		# static toff, for PBOO
		nodes = find_nodes(tree,"all_kernel_args/toff")
		(self.__kernel_toff_value, self.__kernel_toff_unit) = parseTimeNode(nodes[0])

		# b factor, for APTM
		nodes = find_nodes(tree,"all_kernel_args/bFactor")
		self.__kernel_b_factor = nodes[0].attrib['value']

		# offline data path and filename prefix, for APTM
		nodes = find_nodes(tree,"all_kernel_args/offlinedata/prefix")
		self.__kernel_offline_prefix = nodes[0].attrib['value']


		print('init config end ...')



	def get_kernel_type (self):
		return	self.__kernel_type.lower()


	def get_write_file_path(self):
		return self.__write_file_path

	def get_csv_filepath(self):
		return self.__csv_path.get_final_path()

	def get_filepath(self):
		return self.__filepath

	def get_relative_deadline(self):
		return self.__relative_deadline_value

	def get_exe_factor(self):
		return self.__exe_factor

	def get_kernel_period(self):
		return self.__kernel_period_value

	def get_b_factor(self):
		return self.__kernel_b_factor

	def get_kernel_ton(self):
		return self.__kernel_ton_value

	def get_kernel_toff(self):
		return self.__kernel_toff_value

	def get_event_period(self):
		return self.__event_period_value

	def get_event_jitter(self):
		return self.__event_jitter_value

	def get_event_distance(self):
		return self.__event_distance_value

	def get_event_wcets(self):
		return self.__event_wcets_value

	'''***********************************************'''
	def set_simulation_duration(self, duration):
		self.__duration_value = duration

	def set_xml_csv_file_prefix(self, prefix):
		self.__xmlfileprefix = prefix
		self.__csvfileprefix = prefix
		if self.__kernel_type in self.valid_kernels:
			self.update_xml_csv_filenames()

	def set_kernel_type(self, kernel_type):
		self.set_kernel(kernel_type)

	def set_xml_csv_sub_dir(self, subdir):
		self.set_xml_sub_dir(subdir)
		self.set_csv_sub_dir(subdir)

	def set_xml_sub_dir(self, subdir):
		self.__xml_path.set_sub_dir(subdir)

	def set_csv_sub_dir(self, subdir):
		self.__csv_path.set_sub_dir(subdir)

	def set_xml_filename(self, filename):
		self.__xml_path.set_file_name(filename)

	def set_csv_filename(self, filename):
		self.__csv_path.set_file_name(filename)

	def set_relative_deadline(self, relative_deadline):
		self.__relative_deadline_value = relative_deadline

	def set_exe_factor(self, exe_factor):
		self.__exe_factor = exe_factor

	def set_kernel_period(self, kernel_period):
		self.__kernel_period_value = kernel_period

	def set_b_factor(self,b_factor):
		self.__kernel_b_factor = b_factor

	def set_kernel_ton(self,ton):
		self.__kernel_ton_value = ton

	def set_kernel_toff(self,toff):
		self.__kernel_toff_value = toff

	def set_event_period(self,event_period):
		self.__event_period_value = event_period

	def set_event_jitter(self,event_jitter):
		 self.__event_jitter_value = event_jitter

	def set_event_distance(self,event_distance):
		 self.__event_distance_value = event_distance

	def set_event_wcets(self,event_wcets):
		 self.__event_wcets_value = event_wcets



		
if __name__ == "__main__": 
	config_me = Config("test")
	config_me.set_kernel('aptM')
	b = copy.copy(config_me)
	b.set_kernel('pboo')
	attrs = vars(config_me)
	print '\n'.join("%s: %s" % item for item in attrs.items())
	config_me.set_xml_sub_dir('test/')
	config_me.set_xml_filename('testtt2')
	config_me.save_to_xml()


