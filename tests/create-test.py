import random 
import os

def createfile(path):
	file = open(path+'.txt','w')
	i = int(random.random() * 20)
	for line in range(2000 * i):
		a = int(6 + random.random()*4)
		for charr in range( 60 + int(random.random()*50) - int(random.random()*50)):
			if charr % a == int(random.random() * 7):
				file.write(" ")
			else:
				file.write(chr(33 + int(random.random()*90)))
			#print(charr)
		file.write("\n")
	file.close()

os.system("mkdir test")
os.system("mkdir test/files")
for i in range(5):
	os.system("mkdir test/files/"+str(i))
	for j in range(5):
		createfile("test/files/"+str(i)+"/"+str(j))
	for j in range(5):
		os.system("mkdir test/files/"+str(i)+"/"+str(j) )
		for k in range(5):
			createfile("test/files/"+str(i)+"/"+str(j)+"/"+str(k))
		for k in range(5):
			os.system("mkdir test/files/"+str(i)+"/"+str(j)+"/"+str(k))
			for n in range(5):
				createfile("test/files/"+str(i)+"/"+str(j)+"/"+str(k)+"/"+str(n))
