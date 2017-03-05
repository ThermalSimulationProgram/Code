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
		delete_file_folder(dir_name)
		os.mkdir(dir_name)
def change_filename(tree,new_filepath,new_filename):
	# print(new_filename)
	base_path = 'result/csv/'
	new_filename = base_path+new_filename
	tree_root = tree.getroot()
	# print tree_root.attrib
	tree_root.set('name',new_filename)
 
	write_xml(tree,"".join([new_filepath,'.xml']))

def change_relative_deadline():
	base_value = 120
	read_filename = 'example_aptm'
	filename = 'relative_deadline_'
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
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'relative_deadline_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_exe_factor():
	base_value = 0.1
	read_filename = 'example_aptm'
	filename = 'exe_factor_'
	# filepath = "".join(['./',filename,'.xml'])
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
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'exe_factor_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_event_period():
	base_value = 100
	read_filename = 'example_aptm'
	filename = 'event_peroid_'
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/event_peroid/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "events/event_peroid") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(base_value)})  

	new_value = base_value
	for x in range(0,1):
		new_value = new_value+ x*10
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'event_peroid_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_event_Jitter():
	base_value = 150
	read_filename = 'example_aptm'
	filename = 'event_Jitter_'
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/event_Jitter/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "events/event_Jitter") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(base_value)})  

	new_value = base_value
	for x in range(0,1):
		new_value = base_value+ x*10
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'event_Jitter_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_event_distance():
	base_value = 0
	read_filename = 'example_aptm'
	filename = 'event_distance_'
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/event_distance/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "events/event_distance") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(base_value)})  

	new_value = base_value
	for x in range(0,1):
		new_value = base_value+ x*10
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'event_distance_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_event_wcets():
	base_value = "{14.2 9 3.6 5.7}"
	read_filename = 'example_aptm'
	filename = 'event_wcets_'
	# filepath = "".join(['./',filename,'.xml'])
	dirname = 'result/xml/event_wcets/'
	make_dir(dirname)
	tree = read_xml("".join([read_filename,'.xml']))  

	nodes = find_nodes(tree, "events/event_wcets") 
	result_nodes = get_node_by_keyvalue(nodes, {"value":str(base_value)})  

	new_value = base_value
	for x in range(0,1):
		new_value = base_value
		new_filename = filename+str(new_value)
		new_filepath = "".join([dirname,new_filename])
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'event_wcets_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_kernel_period():
	base_value = 10
	read_filename = 'example_aptm'
	filename = 'kernel_period_'
	# filepath = "".join(['./',filename,'.xml'])
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
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'kernel_period_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
def change_kernel_factor():
	base_value = 0.6
	read_filename = 'example_aptm'
	filename = 'kernel_factor_'
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
		change_filename(tree,new_filepath,new_filename)
		change_node_properties(result_nodes, {"value": str(new_value)})  
		
		new_filename = "".join([dirname,'kernel_factor_',str(new_value),'.xml'])
		write_xml(tree,new_filename)

		command_exe = 	'sudo ./main '+new_filename
		print command_exe
		if not no_command:
			os.system(command_exe)
		
if __name__ == "__main__": 
	make_dir('result')
	make_dir('result/xml')
	make_dir('result/csv')

	global no_command
	no_command = False

	change_relative_deadline()
	change_exe_factor()
	change_event_period()
	change_event_Jitter()
	change_event_wcets()
	change_event_distance()
	change_kernel_period()
	change_kernel_factor()

