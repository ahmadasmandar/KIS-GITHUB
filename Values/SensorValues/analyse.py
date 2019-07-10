f = open('Sensorsvalues-1.txt', 'r+')
content=f.readlines()
with open('photovalues-clean.txt','w') as outfile2:
    for x in content:
        # outfile2.write(x)
        if "time delta_photo"in x:
            x=x.replace("time delta_photo","").lstrip()
            outfile2.write(x)
