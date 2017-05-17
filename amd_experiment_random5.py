
from Config import *

randseq=[ 
    [ 2,    10,     4,     5,     3],
    [ 8,     9,     2,    10,     3],
    [ 6,     1,     9,     4,    10],
    [ 9,     2,     5,     3,    10],
    [ 3,     7,     1,     9,     8],
    [ 4,     1,     9,     8,     7],
    [10,     3,     5,     4,     2],
    [ 1,     6,     5,     8,     3],
    [ 2,     4,     9,     1,     7],
    [ 2,    10,     9,     7,     8]];



exe_factor = 0.5;
exefactors = [];
for i in range(0, 5):
	exefactors.append(exe_factor)



c = Config();

c.set_xml_csv_sub_dir('random-five-events/')

df = 1
c.deadline_factor = df
c.duration_value = 180;

control = [0, 0, 1, 1, 1, 1]

for taskids in randseq:
	
	c.set_xml_csv_file_prefix('S' + taskIds2name(taskids))
	(streams, deadlines, csvpaths) = getTaskParameterFromLibrary(taskids, df)

	c.set_task_set(streams, deadlines, exefactors, csvpaths)

	c.run_all_kernels_v2(taskids, control, 120)






