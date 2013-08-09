pylcs
=====

A Python implementation of [Learning Classifier Systems](http://code.ulb.ac.be/dbfiles/HolBooCol-etal2000lcs.pdf).

Implemented underneath in C++ and integraged via Cython. *Currently their is a memory leak issue*.

Here is an example solving the 6-multiplexer problem...

![Test run of 6-multiplexor](6multiplex.png)


```python
import pylcs
import matplotlib.pyplot as plt
from random import randint

# Define muliplexer 'fitness' function
def multiplexer(bits,addrlen=2):
  return bits[int(reduce(lambda x,y: str(y)+str(x),bits[0:addrlen]),base=2)]

# Set up to record time, performance and population
t = [] 
p = []
s = []

# Create classifier by defining the action set [0 or 1]
lcs = pylcs.xcs([0,1])

while True:
  # Record current values
  t.append(lcs.getTime())
  p.append(lcs.getInternalPerformance())
  s.append(lcs.getPopulationSize())
  # Generate random perception of 6 bits
  percept = [randint(0,1) for b in range(1,6+1)]
  # Determine classifier action based on this
  act = lcs.takeAction(percept)
  # Give reward if action is correct
  if (act==multiplexer(percept)):
    lcs.giveReward(1000)
  # Terminate if run too long or performance good
  if t[-1]==1000: # or p[-1]<0.1: 
    break

# Plot results
plt.subplot(2,1,1)
plt.plot(t,p,'r-'); plt.ylabel('Performance')
plt.subplot(2,1,2)
plt.plot(t,s,'b-'); plt.ylabel('Population')
plt.xlabel('Time')
plt.show()
```

Only the [eXtendend Classifier System (XCS)](http://link.springer.com/content/pdf/10.1007/s005000100111.pdf) is currently implemented.

Make sure you have cython installed - e.g. `pip install cython`

Then build en-situ with:

```	
python setup.py build_ext --inplace
```

Or install with:

```
python setup.py install
```

This original code was written back in 2002 for my Master's thesis ["Dynamically Developing Novel and Useful Behaviours: a First Step in Animat Creativity"](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.10.7447&rep=rep1&type=pdf).
