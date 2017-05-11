
import random
from xml_api import *

def UUniFast(n, U):
	sumU = U;
	vectU = []
	for i in range(1, n):
		r = random.random()**(1.0/(n - i))
		nextSumU = sumU*r
		vectU.append(sumU - nextSumU)
		sumU = nextSumU

	vectU.append(sumU)
	return vectU

def getTaskParameterFromLibrary(taskids, deadlinefactor):
	stream_lib = [  [ 198,   387,    48,    12],
				[102,    70,    45,     7],
				[283,   269,    58,     7],
				[354,   387,    17,    11],
				[239,   220,    65,     8],
				[194,   260,    32,     5],
				[148,    91,    78,    13],
				[114,    13,     1,    14],
				[313,   302,    86,     5],
				[119,   187,    89,     6]];
	streams = [];
	deadlines = [];
	csvpaths = [];
	for id in taskids:
		streams.append(stream_lib[id-1])
		deadlines.append(stream_lib[id-1][0]*1.0*deadlinefactor)
		csvpaths.append('./csv_data/S'+str(id) + '_event_trace_200_second.csv') ;

	return (streams, deadlines, csvpaths)



class Task(object):
	def __init__(self, _period, _utilization, _nstage, _name, _jitter = 0, _distance = 0, _deadline = 100, _exefactor = 1, _csvpath = ''):
		self.name = _name
		self.type = "pipelined"
		self.periodicity = "periodic"
		self.load_type = "busy_wait"

		self.benchmark_name = "default"
		self.user_defined_load_id = 1;

		self.period = _period
		self.u = _utilization
		self.nstage = _nstage
		self.jitter = _jitter
		self.distance = _distance
		self.deadline = _deadline;
		self.exefator = _exefactor;
		self.csvpath = _csvpath;
		self.attached_cores = list(range(0, _nstage))
		
		

		wcets = []
		for i in range(0, _nstage):
			w = int(_period*_utilization+0.5) 
			if (w<1):
				w = 1
			wcets.append(w)
		self.wcets = wcets

	def to_xml_node(self):
		task_node = create_node(str(self.name), {'type':str(self.type), 'periodicity':self.periodicity, 'load_type':self.load_type,'name':str(self.name)}, "")
		csvpath = create_time_node("csv_path", self.csvpath, "ms");

		period = create_time_node("period", self.period, "ms");
		jitter = create_time_node("jitter", self.jitter, "ms");
		distance = create_time_node("distance", self.distance, "ms");
		wcets = create_time_node("wcets", self.wcets, "ms");
		deadline = create_time_node("relative_deadline", self.deadline, "ms");
		exefactor = create_node("exe_factor", {'value':str(self.exefator)}, "");

		attached_cores = create_node("attached_cores", {'value':str(self.attached_cores)}, "");

		task_node.append(csvpath)
		task_node.append(period)
		task_node.append(jitter)
		task_node.append(distance)
		task_node.append(wcets)
		task_node.append(deadline)
		task_node.append(exefactor)

		task_node.append(attached_cores)

		if (self.load_type == 'benchmark'):
			benchmark = create_node("benchmark",{'name':self.benchmark_name},"");
			task_node.append(benchmark)
		if (self.load_type == 'user_defined'):
			userload = create_node("defined_load",{'index':str(self.user_defined_load_id)},"");
			task_node.append(userload)

		return task_node


def createTaskSetFromStream(stream, deadlines, exefactors, csvpaths):

	task_num = len(stream);

	if (len(deadlines) != task_num or len(exefactors) != task_num or len(csvpaths) != task_num):
		print "error parameter, sizes do not match with each other"
		return -1;

	taskSet = [];
	name_prefix = 'task'

	for i in xrange(0, task_num):
		name = name_prefix + str(i);
		para = stream[i];
		if (len(para) != 4 ):
			print "incorrect stream"
			return
		newtask = Task(para[0], para[3]*1.0/para[0], 1, name, para[1], para[2], deadlines[i], exefactors[i], csvpaths[i])
		taskSet.append(newtask)

	return taskSet
			
def createTaskSet(task_num, period, utilization, _nstage):
	# task_num = random.randint(1, 10);

	vectU = UUniFast(task_num, utilization);
	taskSet = [];
	name_prefix = 'task'

	for i in xrange(0, task_num):
		name = name_prefix + str(i)
		newtask = Task(period, vectU[i], _nstage, name)
		taskSet.append(newtask)

	return taskSet





if __name__ == "__main__":
	taskset = createTaskSet(100, 0.7, 2)

	allu = 0
	for task in taskset:
		allu = allu + task.u
		print task.u
		print task.wcets

	print allu


