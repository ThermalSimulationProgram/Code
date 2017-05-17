
from Config import *

randseq=[ 
     [6,     3,     8,     7],
     [6,     1,     7,     4],
     [2,    10,     8,     9],
     [5,     2,     1,     7],
     [5,     4,    10,     1],
     [7,     4,     8,     2],
     [9,     2,     1,     6],
     [1,     5,     7,     3],
     [6,    10,     5,     3],
     [8,     4,     9,     1]];



exe_factor = 0.5;
exefactors = [];
for i in range(0, 4):
	exefactors.append(exe_factor)



c = Config();

c.set_xml_csv_sub_dir('random-four-events/')

df = 0.3
c.deadline_factor = df
c.duration_value = 120;

control = [0, 0, 1, 1, 1, 1]

for taskids in randseq:
	
	c.set_xml_csv_file_prefix('S' + taskIds2name(taskids))
	(streams, deadlines, csvpaths) = getTaskParameterFromLibrary(taskids, df)

	c.set_task_set(streams, deadlines, exefactors, csvpaths)

	c.run_all_kernels_v2(taskids, control, 0.01, True)







