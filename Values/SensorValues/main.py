import numpy as np
from matplotlib import pyplot as plt
p = open('photovalues-clean.txt', 'r+')
h = open('hallvalues-clean.txt', 'r+')
content_photo=p.readlines()
x=[]
i=0
for val in content_photo:
    x.append(int(val.rstrip("\n")))
    i+=1
y=(i+1)*30
time_1=np.arange(30,y,30)
print(len(x))
print(len(time_1))
plt.plot(time_1,x)
plt.ylim(0,400)
plt.xlim(0,5000)
plt.grid(True)
plt.xlabel('theta')
plt.ylabel('time delta ühoto')
plt.show()