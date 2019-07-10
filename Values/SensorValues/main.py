#Written by Ahmad Asmandar 2019
import numpy as np
import matplotlib.patches as mpatches
from matplotlib import pyplot as plt
p = open('photovalues-clean.txt', 'r+')
h = open('hallvalues-clean.txt', 'r+')
content_photo=p.readlines()
content_hall=h.readlines()
x=[]
y=[]
i=0
j=0
# for val in content_photo:
#     x.append(int(val.rstrip("\n")))
#     i+=1

for val2 in content_hall:
    y.append(int(val2.rstrip("\n")))
    j+=1

y_plot=(j+1)*180
time_1=np.arange(180,y_plot,180)
# y_plot=(i+1)*30
# time_1=np.arange(30,y_plot,30)
sum=0
k=1
for val3 in y:
    if val3 < 400:
        sum=sum+val3
        k+=1
average=sum/k
# print(y)
print(sum)
print (len(y))
print(average)
plt.axhline(y=average,color='r',linewidth=2)
print(len(time_1))
plt.plot(time_1,y)
plt.xlim(0,10000)
plt.ylim(0,1000)
plt.grid(True)
red_patch = mpatches.Patch(color='red', label="average: {}".format(round(average)))
plt.legend(handles=[red_patch])
plt.title("Hall Sensor  --  KISG6")
plt.xlabel('theta degree')
plt.ylabel("time delta hall in ms")
plt.show()



