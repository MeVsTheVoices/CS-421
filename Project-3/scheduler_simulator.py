#imports to allow for an abstract base class
from abc import ABC, abstractmethod
from os import get_terminal_size
from math import ceil

class SchedulerStrategy:
	"""Base class to allow polymorphism for scheduling strategies"""
	@abstractmethod
	#only method here, ensure children implement functionality
	def do_simulation(self, processes):
		pass

class FirstComeFirstServe(SchedulerStrategy):
	#simplest method, sort by arrival time
	def do_simulation(self, processes):
		results = []
		readyQueue = processes
		readyQueue.sort(key= lambda x: (x[2]))
		for i in readyQueue:
			results.append([i[0], i[1]])
		return results
	
class ShortestJobFirst(SchedulerStrategy):
	def do_simulation(self, processes):
		results = []
		readyQueue = processes
		#sort by burst then arrival time
		readyQueue.sort(key= lambda x: (x[1], x[2]))
		elapsed = 0
		#use to disreguard those that haven't arrived
		oneWasntFinished = True
		while (oneWasntFinished):
			oneWasntFinished = False
			for i in readyQueue:
				#the only jobs we inspect are those still with run times
				#that are greater than zero and that have arrived at
				#the scheduler in time to be dispatched
				if (elapsed >= i[2]) and (i[1] > 0):
					results.append([i[0], i[1]])
					elapsed = elapsed + i[1]
					i[1] = 0
					oneWasntFinished = True
					break
		return results
					
		
class RoundRobin(SchedulerStrategy):
	#keep the quantum as a class variable
	def __init__(self, quantum):
		super().__init__()
		self.quantum = quantum

	#per the round robin strategy, build a list with corresponding
	#times for which the process run, and the sequence in which they ran
	def do_simulation(self, processes):
		if (len(processes) <= 0):
			return
		
		#create a place for results and stable sort
		#the ready queue on arrival times
		results = []
		readyQueue = processes
		readyQueue.sort(key= lambda x: (x[2]))
		oneWasntFinished = True

		#we'll continue while no process has remaining burst time
		elapsed = 0
		while (oneWasntFinished):
			oneWasntFinished = False
			for i in readyQueue:
				print(readyQueue)
				#skip over all finished processes
				if i[1] <= 0:
					continue
				#and all that haven't arrived yet
				elif i[2] > elapsed:
					continue
				#if we'd exceed quantum
				elif i[1] >= self.quantum:
					oneWasntFinished = True
					i[1] = i[1] - self.quantum
					results.append([i[0], self.quantum])
					elapsed = elapsed + self.quantum
				#if we'd not exceed quantum
				elif i[1] < self.quantum:
					oneWasntFinished = True
					results.append([i[0], i[1]])
					elapsed = elapsed + i[1]
					i[1] = 0

		return results
		
	
#returns the processes in the file as a list of lists
def get_processes_from_file(filename):
	processes = []
	f = open(filename)
	for x in f:
		name, burst, arrival = x.split()
		#convert numeric values for use later
		processes.append([name, int(burst), int (arrival)])
	return processes

def summary_from_results(results):
	#form an empty list
	summary = []
	#grab only the names of the processes
	processNames = list({c[0] for c in results})
	#iterate on this
	for name in processNames:
		waitingTime = 0
		executionTime = 0
		wasLast = 0
		#use only execution times and sequence to rebuild
		for result in results:
			if name != result[0]:
				waitingTime = waitingTime + result[1]
			else:
				executionTime = executionTime + result[1]
				wasLast = waitingTime
		#little arithmetic to ignore any inspected results
		#that weren't part of the processes wait time
		waitingTime = (wasLast - waitingTime) + waitingTime
		summary.append([name, waitingTime, executionTime, waitingTime + executionTime])
	#give back an answer sorted by names
	summary.sort(key = lambda x: (x[0]))
	return summary

def print_gant_chart(results):
	#base the horizontal size roughly on the scale of the terminal
	columns, _ = get_terminal_size()
	#columns - 2 * len corrects for extra | characters
	#and the names of the processes (characters)
	columns = (columns - 2*len(results)) * 0.75
	totalTime = 0
	for i in results:
		totalTime = totalTime + i[1]
	totalSegments = len(results)
	print('|', end="")
	for i in results:
		#get ourselves the length of a segment and base
		#our outputs on that length
		at = 0
		limit = ceil(columns*(i[1]/totalTime))
		half = limit // 2
		for j in range(limit):
			#print the name of the process in the middle of the segment
			if at == half:
				print(i[0], end="")
			#print a character to space everything else
			else :
				print('-', end="")
			at = at + 1
		print("|", end="")
	print()

				

#run through the simulation so far and print out results
def do_simulation(strategy):
	results = strategy.do_simulation(get_processes_from_file("Prog3inputfileS23.txt"))
	summary = summary_from_results(results)
	print("{:<9} {:<9} {:<9} {:<9}".format(
		"name", "waiting", "execution", "turnaround"
	))
	for i in summary:
		print("{:<9} {:<9} {:<9} {:<9}".format(i[0], i[1], i[2], i[3]))
	print_gant_chart(results)
