import sys
sys.path.append('..')

import pylcs
import matplotlib.pyplot as plt

def local_binary_pattern():
  pass


# Training data from: http://www.cse.oulu.fi/CMV/ImageData

# Set up to record time, performance and population
t = [] 
p = []
s = []

# Create classifier by defining the action set [0 or 1]
lcs = pylcs.xcs([0,1])

# Train classifier on labeled 

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
  if (act==local_binary_pattern(percept)):
    lcs.giveReward(1000)
  # Terminate if run too long or performance good
  if t[-1]==1000: # or p[-1]>0.7: 
    break

# Plot results
plt.subplot(2,1,1)
plt.plot(t,p,'r-'); plt.ylabel('Performance')
plt.subplot(2,1,2)
plt.plot(t,s,'b-'); plt.ylabel('Population')
plt.xlabel('Time')
plt.show()

# Now, apply classifier to new images...


