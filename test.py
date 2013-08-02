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
