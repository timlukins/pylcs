pylcs
=====

A Python implementation of Learning Classifier Systems built ontop of C++ code via Cython. 

Example for the 6-multiplexer problem:

				import pylcs
				import matplotlib.pyplot as plt
				from random import randint

				def multiplexer(bits,addrlen=2):
					return bits[int(reduce(lambda x,y: str(y)+str(x),bits[0:addrlen]),base=2)]

				lcs = pylcs.xcs([0,1]) # create & pass in the action set

				t = p = s = [] 
				while True:
					t.append(lcs.getTime())
					p.append(lcs.getInternalPerformance())
					s.append(lcs.getPopulationSize())
					percept = [randint(0,1) for b in range(1,6+1)]
					act = lcs.takeAction(percept)
					if (act==multiplexer(percept)):
						lcs.giveReward(1000)
					if p[-1]<0.0001:
						break

				plt.plot(t,p,'r+')
				plt.plot(t,s,'b-')
	
Only the eXtendend Classifier System (XCS)[1] is currently implemented.

Either build en-situ with:
	
				> python setup.py build_ext --inplace

Or install with:

				> python setup.py

	[1] http://link.springer.com/content/pdf/10.1007/s005000100111.pdf
