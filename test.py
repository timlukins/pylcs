import sys # Wee trick when running in same directory as module
sys.path.append('..')

import pylcs
import matplotlib.pyplot as plt
from random import randint

# Define muliplexor 'fitness' function
def multiplex(bits,addrlen=2):
  return bits[int(reduce(lambda x,y: str(y)+str(x),bits[0:addrlen]),base=2)]

# Create classifier by defining the action set [0 or 1]
lcs = pylcs.xcs([0,1])

# Set up to record time, performance and population
t = []; p = []; s = []

# Change the learning rate aand exploration probablity...
lcs.BETA = 0.015
lcs.EPSILON = 0.1

while True:
  # Record current values as we go
  t.append(lcs.time())
  p.append(lcs.perf())
  s.append(lcs.size())
  # Generate random perception of 6 bits
  percept = [randint(0,1) for b in range(1,6+1)]
  # Determine classifier action based on this
  act = lcs.act(percept)
  # Give reward if action is correct
  if (act==multiplex(percept)):
    lcs.reward(1000)
  # Terminate if run too long or performance good
  if t[-1]>1000 and p[-1]>0.6: 
    break

# Plot results
ax1 = plt.subplot(2,1,1)
plt.plot(t,p,'r-'); plt.ylabel('Performance'); ax1.set_ylim([0,1.0])
ax2 = plt.subplot(2,1,2)
plt.plot(t,s,'b-'); plt.ylabel('Population')
plt.xlabel('Time')
plt.show()
