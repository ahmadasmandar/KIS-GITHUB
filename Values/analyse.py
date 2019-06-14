f = open('output.txt', 'r+')
content=f.readlines()
with open('output2.txt','w') as outfile2:
    for x in content:
        outfile2.write(x)
        if "trigger pressed"in x:
            outfile2.write("*****************************************"+"\n")
