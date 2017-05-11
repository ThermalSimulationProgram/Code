
from Config import *

taskids = list(range(1,11));

deadline_factors = [];

for i in range(5, 6):
	deadline_factors.append(i*0.1)

exe_factor = 1;
exefactors = [];
for i in range(0, len(taskids)):
	exefactors.append(exe_factor)



c = Config();

c.set_xml_csv_sub_dir('test_config/')

c.duration_value = 100;

control = [0, 0, 1, 1, 1, 1]

for df in deadline_factors:
	c.deadline_factor = df
	c.set_xml_csv_file_prefix(str(df))
	(streams, deadlines, csvpaths) = getTaskParameterFromLibrary(taskids, df)
	c.set_task_set(streams, deadlines, exefactors, csvpaths)

	c.run_all_kernels_v2(taskids, control, 100)






