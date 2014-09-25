# STL vector
from libcpp.vector cimport vector

###############################################################################


# namespace
cdef extern from "LCS_XCS.h" namespace "LCS":

	cdef cppclass XCS: 
	  # Constructor	
		XCS(vector[long])
		# public variables
		double BETA		# Learning rate.
		double GAMMA		# Discount factor.
		double ALPHA   # Adjustment in fitness calculation.
		long   ERROR   # Value below which classifiers equal
		double VAL			# Power parameter in fitness calculation.
		double EPSILON # Exploration probability.
		long   N			  #  Maxsize of population (zero = no limit)
		double MU			# Probability of mutation.
		double XU			# Probability of crossover.
		double SIGMA		# Mimimim fitness level.
		double PHASH   # Probability of # when covering.
		long   THETAGA  # GA application threshold
		long   THETADEL # GA culling threshold
		long   THETASUB # GA subsumption threshold
		long   THETAACT # Minimum actions in matchset before covering.
		# Methods	
		long act(vector[int])
		void update(long)

		long populationSize()
		double internalPerformance()
		unsigned long currentTime()
	
		void subsumptionOn()
		void subsumptionOff()
		void learningOn()
		void learningOff()

###############################################################################

cdef class xcs:
	cdef XCS *thisptr      # hold a C++ instance which we're wrapping
	
	def __cinit__(self,actions):
		self.thisptr = new XCS(actions)

	def __dealloc__(self):
		del self.thisptr
	
	def time(self):
		return self.thisptr.currentTime()
	
	def perf(self):
		return self.thisptr.internalPerformance()
	
	def size(self):
		return self.thisptr.populationSize()
	
	def act(self,perception):
		cdef vector[int] vect = list(perception)
		return self.thisptr.act(vect)

	def reward(self,amount):
		self.thisptr.update(amount)

	def doSubsumption(self,yes):
		if yes:
			self.thisptr.subsumptionOn()
		else:
			self.thisptr.subsumptionOff()
  
	def doLearning(self,yes):
		if yes:
			self.thisptr.learningOn()
		else:
			self.thisptr.learningOff()

	property BETA:
		def __get__(self): return self.thisptr.BETA
		def __set__(self,beta): self.thisptr.BETA = beta
	
	property GAMMA:
		def __get__(self): return self.thisptr.GAMMA
		def __set__(self,gamma): self.thisptr.GAMMA = gamma 
	
	property ALPHA:
		def __get__(self): return self.thisptr.ALPHA
		def __set__(self,alpha): self.thisptr.ALPHA = alpha 
	
	property ERROR:
		def __get__(self): return self.thisptr.ERROR
		def __set__(self,error): self.thisptr.ERROR = error 
	
	property VAL:
		def __get__(self): return self.thisptr.VAL
		def __set__(self,val): self.thisptr.VAL = val 
	
	property EPSILON:
		def __get__(self): return self.thisptr.EPSILON
		def __set__(self,epsilon): self.thisptr.EPSILON = epsilon 
	
	property N:
		def __get__(self): return self.thisptr.N
		def __set__(self,n): self.thisptr.N = n 
	
	property MU: 
		def __get__(self): return self.thisptr.MU
		def __set__(self,mu): self.thisptr.MU = mu 
	
	property XU: 
		def __get__(self): return self.thisptr.XU
		def __set__(self,xu): self.thisptr.XU = xu 
	
	property SIGMA: 
		def __get__(self): return self.thisptr.SIGMA
		def __set__(self,sigma): self.thisptr.SIGMA = sigma 
	
	property PHASH: 
		def __get__(self): return self.thisptr.PHASH
		def __set__(self,phash): self.thisptr.PHASH = phash 
	
	property THETAGA: 
		def __get__(self): return self.thisptr.THETAGA
		def __set__(self,thetaga): self.thisptr.THETAGA = thetaga 
	
	property THETADEL: 
		def __get__(self): return self.thisptr.THETADEL
		def __set__(self,thetadel): self.thisptr.THETADEL= thetadel 
	
	property THETASUB: 
		def __get__(self): return self.thisptr.THETASUB
		def __set__(self,thetasub): self.thisptr.THETASUB = thetasub
	
	property THETAACT: 
		def __get__(self): return self.thisptr.THETAACT
		def __set__(self,thetaact): self.thisptr.THETAACT = thetaact 
	
