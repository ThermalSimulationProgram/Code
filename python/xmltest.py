# -*- coding=utf-8 -*-  
# python2.7
import os
from xml.etree.ElementTree import ElementTree,Element  

#from xml.dom import minidom

import xml.dom.minidom



  
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
    return nodelist
          
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
		print('{dir} exists ...'.format(dir=dir_name))
		# delete_file_folder(dir_name)
		# os.mkdir(dir_name)
def make_dirs(dir_name):
	if not os.path.exists(dir_name):
		os.makedirs(dir_name)
	else:
		print('{dir} exists ...'.format(dir=dir_name))


def create_time_node(name, value, unit):
	node = create_node(name, {'value':str(value), 'unit':unit }, "")
	return node

if __name__ == "__main__": 
	tree = ElementTree()
	simulation = create_node('simulation', { 'name': "testxml2" }, "")
	length = 70
	unit = 'ms'
	duration = create_time_node('duration', length, unit);
	pipeline = create_node('pipeline', {'stagenumber':str(length)}, "")



	events = create_node('events', {}, "")
	csv_path = create_node('csv_path', {'value':str(length)}, "")
	period = create_time_node('period', length, unit);
	distance = create_time_node('distance', length, unit);
	jitter = create_time_node('jitter', length, unit);
	wcets = create_time_node('wcets', length, unit);
	deadline = create_time_node('relative_deadline', length, unit);
	exe_factor = create_node('exe_factor', {'value':str(length)}, "")


	events.append(csv_path)
	events.append(period)
	events.append(jitter)
	events.append(distance)
	events.append(wcets)
	events.append(deadline)
	events.append(exe_factor)


	scheduler = create_node('events', {}, "")
	

	
	simulation.append(duration)
	simulation.append(pipeline)
	simulation.append(events)


	tree._setroot(simulation)


	write_xml(tree, "testxml.xml")

	xml = xml.dom.minidom.parse("testxml.xml") # or xml.dom.minidom.parseString(xml_string)
	pretty_xml_as_string = xml.toprettyxml()
	
	f = open("testxml.xml", "w");

	f.write(pretty_xml_as_string)

	
		




