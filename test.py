import sys
sys.path.append('..')

import pylcs
import matplotlib.pyplot as plt
from random import randint

# Define muliplexor 'fitness' function
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
