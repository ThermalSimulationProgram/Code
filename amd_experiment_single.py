
from Config import *

taskids = list(range(1,11));


exe_factor = 1;
exefactors = [];
for i in range(0, 1):
	exefactors.append(exe_factor)



c = Config();

c.set_xml_csv_sub_dir('single-ten-events/')

df = 1
c.deadline_factor = df
c.duration_value = 180;

control = [0, 0, 1, 1, 1, 1]

for theid in taskids:
	
	c.set_xml_csv_file_prefix('Stream_' + str(theid))
	(streams, deadlines, csvpaths) = getTaskParameterFromLibrary([theid], df)
	c.set_task_set(streams, deadlines, exefactors, csvpaths)

	c.run_all_kernels_v2([theid], control, 120)






