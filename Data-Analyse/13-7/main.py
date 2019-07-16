
#https://plot.ly/matplotlib/polynomial-fits/
#"c:/Users/Ahmad Asmandar/Desktop/KIS-GITHUB/Data-Analyse/13-7/main.py"
from matplotlib import pyplot as plt
import numpy as np
f = open('100 % photo', 'r+')
content=f.readlines()
target_time=[]
angular_speed=[]
deference=[]
# with open('data/target_time_addition_value_photo_1.txt','w') as outfile2:
for x in content:
    # outfile2.write(x)
    if "target_time"in x:
        # x=x.replace("time delta_photo","").lstrip()
        # outfile2.write(x)
        target_time.append(float(x.replace("target_time","").replace("ms","").lstrip().rstrip("\n")))
    if "w rad/s" in x:
        # outfile2.write(x)
        angular_speed.append(float(x.replace("w rad/s","").lstrip().rstrip("\n")))
target_time.sort()
angular_speed.sort(reverse=True)
print(target_time)
# print(len(target_time))
# print(len(angular_speed))
print(angular_speed)
i=0
for val in target_time:
    relation=(val-391)
    deference.append(round(relation,2))
    i+=1
print(deference)
# plt.plot(target_time,angular_speed, color='r',label="speed")
# plt.plot(target_time,deference,label="defrence")
# calculate polynomial
z = np.polyfit(angular_speed,deference,1)
z1 = np.polyfit(angular_speed,deference,2)
z2 = np.polyfit(angular_speed,deference,3)
f = np.poly1d(z)
f1 = np.poly1d(z1)
f2 = np.poly1d(z2)
new_range=np.linspace(0,30,50)
y_new=f(new_range)
y_new1=f1(new_range)
y_new2=f2(new_range)
print(f)
print(f1)
print(f2)
plt.plot(angular_speed,deference,color='red',label="data")
plt.plot(new_range,y_new,color='g',label=f)
plt.plot(new_range,y_new1,color='b',label=f1)
plt.plot(new_range,y_new2,color='y',label=f2)
plt.legend()
plt.show()