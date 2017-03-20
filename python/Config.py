#coding:utf-8

from xml.etree.ElementTree import ElementTree,Element
import os

class Config(object):
	"""docstring for config"""
	def __init__(self,kernel_type):
		super(Config, self).__init__()
		# self.arg = arg
		# self.__kernel_type = 'APTM'
		# self.__read_file_path = ''
		# self.__write_file_path = ''
		# self.__filename = ''
		# self.__filepath = ''
		self.__kernel_type = kernel_type
		self.__load_xml_config()

	'''***********************************************'''

	def get_kernel_type (self):
		return	self.__kernel_type.lower()

	def get_read_file_path(self):
		return self.__read_file_path

	def get_write_file_path(self):
		return self.__write_file_path

	def get_filename(self):
		return self.__filename

	def get_filepath(self):
		return self.__filepath

	def get_relative_deadline(self):
		return self.__relative_deadline

	def get_exe_factor(self):
		return self.__exe_factor

	def get_kernel_period(self):
		return self.__kernel_period

	def get_b_factor(self):
		return self.__b_factor

	def get_kernel_ton(self):
		return self.__kernel_ton

	def get_kernel_toff(self):
		return self.__kernel_toff

	def get_event_period(self):
		return self.__event_period

	def get_event_jitter(self):
		return self.__event_jitter

	def get_event_distance(self):
		return self.__event_distance

	def get_event_wcets(self):
		return self.__event_wcets

	'''***********************************************'''

	def set_kernel_type(self, kernel_type):
		self.__kernel_type = kernel_type

	def set_read_file_path(self, read_file_path):
		self.__read_file_path = read_file_path

	def set_write_file_path(self, write_file_path):
		self.__write_file_path = write_file_path

	def set_filename(self, filename):
		self.__filename = filename

	def set_filepath(self, filepath):
		self.__filepath = filepath

	def set_relative_deadline(self, relative_deadline):
		self.__relative_deadline = relative_deadline

	def set_exe_factor(self, exe_factor):
		self.__exe_factor = exe_factor

	def set_kernel_period(self, kernel_period):
		self.__kernel_period = kernel_period

	def set_b_factor(self,b_factor):
		self.__b_factor = b_factor

	def set_kernel_ton(self,ton):
		self.__kernel_ton = ton

	def set_kernel_toff(self,toff):
		self.__kernel_toff = toff

	def set_event_period(self,event_period):
		self.__event_period = event_period

	def set_event_jitter(self,event_jitter):
		 self.__event_jitter = event_jitter

	def set_event_distance(self,event_distance):
		 self.__event_distance = event_distance

	def set_event_wcets(self,event_wcets):
		 self.__event_wcets = event_wcets

	'''***********************************************'''


	def __find_nodes(self,tree, path):  
	    '''''查找某个路径匹配的所有节点 
	       tree: xml树 
	       path: 节点路径'''  
	    return tree.findall(path) 

	def __get_node_by_keyvalue(self,nodelist, kv_map):  
	    '''''根据属性及属性值定位符合的节点，返回节点 
	       nodelist: 节点列表 
	       kv_map: 匹配属性及属性值map'''  
	    result_nodes = []  
	    for node in nodelist:  
	        if if_match(node, kv_map):  
	            result_nodes.append(node)  
	    return result_nodes  

	'''***********************************************'''


	def __load_xml_config(self, xml_path='Config.xml'):
		tree = ElementTree()
		tree.parse(xml_path)
		nodes = self.__find_nodes(tree,"path/readFilePath")
		self.__read_file_path = nodes[0].attrib['value']

		nodes = self.__find_nodes(tree,"path/writeFilePath")
		self.__write_file_path = nodes[0].attrib['value']
		
		nodes = self.__find_nodes(tree,"path/filename")
		self.__filename = nodes[0].attrib['value']
		
		nodes = self.__find_nodes(tree,"path/filepath")
		self.__filepath = nodes[0].attrib['value']
		
		# nodes = self.__find_nodes(tree,"arg/kernelType")
		# self.__kernel_type = nodes[0].attrib['value']
		
		nodes = self.__find_nodes(tree, "arg/relativeDeadline")
		self.__relative_deadline = nodes[0].attrib['value']

		nodes = self.__find_nodes(tree,"arg/exeFactor")
		self.__exe_factor = nodes[0].attrib['value']

		nodes = self.__find_nodes(tree,"arg/kernelPeriod")
		self.__kernel_period = nodes[0].attrib['value']

		nodes = self.__find_nodes(tree,"arg/bFactor")
		self.__b_factor = nodes[0].attrib['value']

		nodes = self.__find_nodes(tree,"arg/ton")
		self.__kernel_ton = nodes[0].attrib['value']

		nodes = self.__find_nodes(tree,"arg/toff")
		self.__kernel_toff = nodes[0].attrib['value']

		nodes = self.__find_nodes(tree,"event/eventPeriod")
		self.__event_period = nodes[0].attrib['value']
		nodes = self.__find_nodes(tree,"event/eventJitter")
		self.__event_jitter = nodes[0].attrib['value']
		nodes = self.__find_nodes(tree,"event/eventDistance")
		self.__event_distance = nodes[0].attrib['value']
		nodes = self.__find_nodes(tree,"event/eventWcets")
		self.__event_wcets = nodes[0].attrib['value']

		print('init config end ...')

		
if __name__ == "__main__": 
	config_me = Config()