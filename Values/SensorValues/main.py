#Written by Ahmad Asmandar 2019
import numpy as np
from matplotlib import pyplot as plt
p = open('photovalues-clean.txt', 'r+')
h = open('hallvalues-clean.txt', 'r+')
content_photo=p.readlines()
content_hall=h.readlines()
x=[]
y=[]
i=0
j=0
for val in content_photo:
    x.append(int(val.rstrip("\n")))
    i+=1

# for val2 in content_hall:
#     y.append(int(val2.rstrip("\n")))
#     j+=1
# y_plot=(j+1)*180
# time_1=np.arange(180,y_plot,180)
y_plot=(i+1)*30
time_1=np.arange(30,y_plot,30)
print(len(x))
print(len(time_1))
plt.plot(time_1,x)
plt.ylim(0,1800)
plt.xlim(0,5500)
plt.grid(True)
<<<<<<< HEAD
plt.title("photo_sens_values")
plt.xlabel('theta')
plt.ylabel('time delta ühoto')
plt.show()
=======
plt.title("photo Sensor  --  KISG6")
plt.xlabel('theta degree')
plt.ylabel("time delta photo in ms")
plt.show()

>>>>>>> 369856b1131a44467434f092a059152a96b94f4b
