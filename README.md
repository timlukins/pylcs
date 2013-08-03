pylcs
=====

A Python implementation of [Learning Classifier Systems][http://code.ulb.ac.be/dbfiles/HolBooCol-etal2000lcs.pdf] 

Implemented underneath in C++ and integraged via Cython. 

Here is an example solving the 6-multiplexer problem:

```
import pylcs
import matplotlib.pyplot as plt
from random import randint

def multiplexer(bits,addrlen=2):
	return bits[int(reduce(
								lambda x,y: str(y)+str(x),bits[0:addrlen]
							),base=2)]

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
```

Only the [eXtendend Classifier System (XCS)][http://link.springer.com/content/pdf/10.1007/s005000100111.pdf] is currently implemented.

Make sure you have cython installed - e.g. `pip install cython`

Then build en-situ with:

```	
python setup.py build_ext --inplace
```

Or install with:

```
python setup.py
```

This original code was written back in 2002 for my Master's thesis ["Dynamically Developing Novel and Useful Behaviours: a First Step in Animat Creativity"][http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.10.7447&rep=rep1&type=pdf] 
