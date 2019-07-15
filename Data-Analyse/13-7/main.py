
from matplotlib import pyplot as plt
f = open('100 % photo', 'r+')
content=f.readlines()
target_time=[]
angular_speed=[]
deference=[]
with open('data/target_time_addition_value_photo_1.txt','w') as outfile2:
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
plt.plot(angular_speed,deference)
# plt.legend()
plt.show()