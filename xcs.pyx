# STL vector
from libcpp.vector cimport vector

###############################################################################


# namespace
cdef extern from "LCS_XCS.h" namespace "LCS":

	cdef cppclass XCS: 
		#ctypedef long Reward
	  # Constructor	
		XCS(vector[long])
		# public variables
		double BETA
		double GAMMA
		double EPSILON
		# Methods	
		long act(vector[int])
		void update(long)

		long populationSize()
		double internalPerformance()
		unsigned long currentTime()
		
###############################################################################

cdef class xcs:
	cdef XCS *thisptr      # hold a C++ instance which we're wrapping
	
	def __cinit__(self,actions):
		self.thisptr = new XCS(actions)

	def __dealloc__(self):
		del self.thisptr
	
	def getTime(self):
		return self.thisptr.currentTime()
	
	def getInternalPerformance(self):
		return self.thisptr.internalPerformance()
	
	def getPopulationSize(self):
		return self.thisptr.populationSize()
	
	def takeAction(self,perception):
		cdef vector[int] vect = list(perception)
		return self.thisptr.act(vect)

	def giveReward(self,amount):
		self.thisptr.update(amount)

