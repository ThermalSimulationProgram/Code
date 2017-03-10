# -*- coding=utf-8 -*-  

import os
  
from xml.etree.ElementTree import ElementTree,Element  
  
def read_xml(in_path):  
    '''''读取并解析xml文件 
       in_path: xml路径 
       return: ElementTree'''  
    tree = ElementTree()  
    tree.parse(in_path)  
    return tree  
  
def write_xml(tree, out_path):  
    '''''将xml文件写出 
       tree: xml树 
       out_path: 写出路径'''  
    tree.write(out_path, encoding="utf-8",xml_declaration=True)  
  
def if_match(node, kv_map):  
    '''''判断某个节点是否包含所有传入参数属性 
       node: 节点 
       kv_map: 属性及属性值组成的map'''  
    for key in kv_map:  
        if node.get(key) != kv_map.get(key):  
            return False  
    return True  
  
#---------------search -----  
  
def find_nodes(tree, path):  
    '''''查找某个路径匹配的所有节点 
       tree: xml树 
       path: 节点路径'''  
    return tree.findall(path)  
  
  
def get_node_by_keyvalue(nodelist, kv_map):  
    '''''根据属性及属性值定位符合的节点，返回节点 
       nodelist: 节点列表 
       kv_map: 匹配属性及属性值map'''  
    result_nodes = []  
    for node in nodelist:  
        if if_match(node, kv_map):  
            result_nodes.append(node)  
    return result_nodes  
  
#---------------change -----  
  
def change_node_properties(nodelist, kv_map, is_delete=False):  
    '''''修改/增加 /删除 节点的属性及属性值 
       nodelist: 节点列表 
       kv_map:属性及属性值map'''  
    for node in nodelist:  
        for key in kv_map:  
            if is_delete:   
                if key in node.attrib:  
                    del node.attrib[key]  
            else:  
                node.set(key, kv_map.get(key))  
              
def change_node_text(nodelist, text, is_add=False, is_delete=False):  
    '''''改变/增加/删除一个节点的文本 
       nodelist:节点列表 
       text : 更新后的文本'''  
    for node in nodelist:  
        if is_add:  
            node.text += text  
        elif is_delete:  
            node.text = ""  
        else:  
            node.text = text  
              
def create_node(tag, property_map, content):  
    '''''新造一个节点 
       tag:节点标签 
       property_map:属性及属性值map 
       content: 节点闭合标签里的文本内容 
       return 新节点'''  
    element = Element(tag, property_map)  
    element.text = content  
    return element  
          
def add_child_node(nodelist, element):  
    '''''给一个节点添加子节点 
       nodelist: 节点列表 
       element: 子节点'''  
    for node in nodelist:  
        node.append(element)  
          
def del_node_by_tagkeyvalue(nodelist, tag, kv_map):  
    '''''同过属性及属性值定位一个节点，并删除之 
       nodelist: 父节点列表 
       tag:子节点标签 
       kv_map: 属性及属性值列表'''  
    for parent_node in nodelist:  
        children = parent_node.getchildren()  
        for child in children:  
            if child.tag == tag and if_match(child, kv_map):  
                parent_node.remove(child)  
                          
def delete_file_folder(src):  
    '''delete files and folders''' 
    if os.path.isfile(src):  
        try:  
            os.remove(src)  
        except:  
            pass 
    elif os.path.isdir(src):  
        for item in os.listdir(src):  
            itemsrc=os.path.join(src,item)  
            delete_file_folder(itemsrc)  
        try:  
            os.rmdir(src)  
        except:  
            pass 
def make_dir(dir_name):
	if not os.path.exists(dir_name):
		os.mkdir(dir_name)
	else:
		pass
		# delete_file_folder(dir_name)
		# os.mkdir(dir_name)
def change_filename(tree,new_filepath,new_filename,kernel_type):
	# print(new_filename)
	base_path = 'result/csv/'
	new_filename = base_path+new_filename
	tree_root = tree.getroot()
	# print tree_root.attrib
	tree_root.set('name',new_filename)
 	
 	nodes = find_nodes(tree, "scheduler/kernel") 
	result_nodes = get_node_by_keyvalue(nodes, {"type":"APTM"}) 
	change_node_properties(result_nodes, {"type": kernel_type})
	# print(new_filepath)
	write_xml(tree,"".join([new_filepath,'.xml']))


def change_relative_deadline(kernel_type):
	base_value = 120
	read_filename = "".join(['result/xml/event/','example_',kernel_type.lower()])
	filename = "".join(['relative_deadline_',kernel_type,'_'])
	make_dir('result/csv/relative_deadline')
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/relative_deadline/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "events/relative_deadline") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(base_value)})  

	new_value = base_value
	for x in range(0,1):
		new_value = new_value+x*10
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		print(new_filepath)
		change_filename(tree,new_filepath, "".join(["relative_deadline/",new_filename]),kernel_type)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'relative_deadline_',kernel_type,'_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_exe_factor(kernel_type):
	base_value = 0.1
	read_filename = "".join(['result/xml/event/','example_',kernel_type.lower()])
	filename = "".join(['exe_factor_',kernel_type,'_'])
	# filepath = "".join(['./',filename,'.xml'])
	make_dir('result/csv/exe_factor')
	dirname = 'result/xml/exe_factor/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "events/exe_factor") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(0.9)})  

	new_value = base_value
	for x in range(0,10):
		new_value = base_value+x*0.1
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		change_filename(tree,new_filepath,"".join(["exe_factor/",new_filename]),kernel_type)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'exe_factor_',kernel_type,'_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_event_each(kernel_type,eventname,event_period,event_jitter,event_distance,event_wcets):
	read_filename = "".join(['example_',kernel_type])
	base_period = "100"
	base_jitter = "150"
	base_distance = "0"
	base_wcets = "{14.2 9 3.6 5.7}"
	dirname = 'result/xml/event/'
	make_dir(dirname)

	tree = read_xml("".join([read_filename,'.xml']))  
	nodes = find_nodes(tree, "events/period") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_period})  
	change_node_properties(result_nodes, {"value": str(event_period)})
	write_xml(tree,"".join([dirname,eventname,".xml"]))

	read_filename = "".join([dirname,eventname])
	tree = read_xml("".join([read_filename,'.xml']))  
	nodes = find_nodes(tree,"events/jitter")
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_jitter})  
	change_node_properties(result_nodes, {"value": str(event_jitter)})
	write_xml(tree,"".join([read_filename,".xml"]))

	tree = read_xml("".join([read_filename,'.xml']))  
	nodes = find_nodes(tree,"events/distance")
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_distance})  
	change_node_properties(result_nodes, {"value": str(event_distance)})
	write_xml(tree,"".join([read_filename,".xml"]))

	tree = read_xml("".join([read_filename,'.xml']))  
	nodes = find_nodes(tree,"events/wcets")
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_wcets})  
	change_node_properties(result_nodes, {"value": str(event_wcets)})
	write_xml(tree,"".join([read_filename,".xml"]))
	
	write_xml(tree,"".join([dirname,"example_",kernel_type,".xml"]))

def change_event(eventname,event_period,event_jitter,event_distance,event_wcets):

	read_filename = 'aptm'
	change_event_each(read_filename,eventname,event_period,event_jitter,event_distance,event_wcets)
	read_filename = 'bws'
	change_event_each(read_filename,eventname,event_period,event_jitter,event_distance,event_wcets)
	read_filename = 'pboo'
	change_event_each(read_filename,eventname,event_period,event_jitter,event_distance,event_wcets)

def change_kernel_period(kernel_type):
	base_value = 10
	read_filename = "".join(['result/xml/event/','example_',kernel_type.lower()])
	filename = "".join(['kernel_period_',kernel_type,'_'])
	# filepath = "".join(['./',filename,'.xml'])
	make_dir('result/csv/kernel_period')
	dirname = 'result/xml/kernel_period/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "scheduler/kernel/period") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(base_value)})  

	new_value = base_value
	for x in range(0,10):
		new_value = base_value+ x*10
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		change_filename(tree,new_filepath,"".join(["kernel_period/",new_filename]),kernel_type)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'kernel_period_',kernel_type,'_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_kernel_factor(kernel_type):
	base_value = 0.6
	read_filename = "".join(['result/xml/event/','example_',kernel_type.lower()])
	filename = "".join(['kernel_factor_',kernel_type,'_'])
	make_dir('result/csv/kernel_factor')
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/kernel_factor/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "scheduler/kernel/b_factor") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(0.93)})  

	new_value = base_value
	for x in range(61,97):
		new_value = x*0.01
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		change_filename(tree,new_filepath,"".join(["kernel_factor/",new_filename]),kernel_type)
		print new_value
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'kernel_factor_',kernel_type,'_',str(new_value),'.xml'])
		write_xml(tree,new_filename)
		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)

def change_kernel_ton(kernel_type,new_value):
	base_value = "{10,10,10,10}"
	read_filename = "".join(['result/xml/event/','example_',kernel_type.lower()])
	filename = "".join(['kernel_ton_',kernel_type,'_'])
	make_dir('result/csv/kernel_ton')
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/kernel_ton/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "scheduler/kernel/ton") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_value})  

	change_node_properties(result_nodes, {"value": new_value})
	new_filename = "".join([dirname,'kernel_ton_',kernel_type,'_',str(new_value),'.xml'])
	write_xml(tree,new_filename)
	
	command_exe = 	'sudo ./main '+new_filename
	print command_exe
	if not no_command:
		os.system(command_exe)

def change_kernel_toff(kernel_type,new_value):
	base_value = "{30,20,20,20}"
	read_filename = "".join(['result/xml/event/','example_',kernel_type.lower()])
	filename = "".join(['kernel_toff_',kernel_type,'_'])
	make_dir('result/csv/kernel_toff')
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/kernel_toff/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "scheduler/kernel/toff") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":base_value})  

	change_node_properties(result_nodes, {"value": new_value})
	new_filename = "".join([dirname,'kernel_toff_',kernel_type,'_',str(new_value),'.xml'])
	write_xml(tree,new_filename)
	
	command_exe = 	'sudo ./main '+new_filename
	print command_exe
	if not no_command:
		os.system(command_exe)

def test_different_relative_deadline():
	change_relative_deadline("APTM")
	change_relative_deadline("BWS")
	change_relative_deadline("PBOO")	

def test_different_exe_factor():
	change_exe_factor("APTM")
	change_exe_factor("BWS")
	change_exe_factor("PBOO")


if __name__ == "__main__": 
	make_dir('result')
	make_dir('result/xml')
	make_dir('result/csv')
	global event 
	event = "event1"
	
	global no_command
	no_command = False
	# no_command = True

	change_event(event,120,151,1,"{14.2 10 3.6 5.7}")

	test_different_relative_deadline()
	# test_different_exe_factor()

	# change_kernel_period("APTM")
	# change_kernel_period("BWS")
	

	# change_kernel_factor("APTM")
	
	# change_kernel_ton("PBOO","{10,10,10,10}")

	# change_kernel_toff("PBOO","{10,10,10,10}")



