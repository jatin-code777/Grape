import random 
import os
import subprocess
import string
#batcmd="dir"
#result = subprocess.check_output(cmd, shell=True)
t  = 0 

def get_pattern():
	a = random.random()*9 + 1
	if a > 0 and a < 70:
		a = 3
		pattern = ""
		for i in range(a):
			pattern += random.choice(string.ascii_letters)
		return pattern
	elif a > 7:
		a = (int(a) - 3)
		pattern = ""
		for i in range(a):
			pattern += random.choice(string.ascii_letters)
		return pattern


def testfile(path):
	global t
	path = path + ".txt"
	for i in range(5):
		pat = get_pattern()
		cmd1 = "grep -F '"+ pat +"' "+path+ " > grep.txt" #+ " | wc -l"
		result = subprocess.Popen(cmd1, shell=True)
		retval = result.wait()
		cmd2 = "grape -F '"+ pat +"' "+path+ " > grape.txt" #+ " | wc -l"
		p1 = subprocess.Popen(cmd2 ,shell=True ,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		retval = p1.wait()
		cmd = "diff grep.txt grape.txt"
		p2 = subprocess.Popen(cmd, shell=True , stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		retval = p2.wait()
		if(retval > 0):
			print(cmd1)
			print(cmd2)
			print(cmd)
			print(retval)
		#print(retval)
		t = t + retval
		i = i + 1
		cmd = "rm grep.txt grape.txt"
		p = subprocess.Popen(cmd, shell=True , stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		retval = p.wait()
	for i in range(5):
		pat = get_pattern()
		cmd1 = "grep -G '"+ pat +"' "+path+ " > grep.txt" #+ " | wc -l"
		result = subprocess.Popen(cmd1, shell=True)
		retval = result.wait()
		cmd2 = "grape -G '"+ pat +"' "+path+ " > grape.txt" #+ " | wc -l"
		p1 = subprocess.Popen(cmd2 ,shell=True ,stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		retval = p1.wait()
		cmd = "diff grep.txt grape.txt"
		p2 = subprocess.Popen(cmd, shell=True , stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		retval = p2.wait()
		if(retval > 0):
			print(cmd1)
			print(cmd2)
			print(cmd)
			print(retval)
		t = t + retval
		i = i + 1
		cmd = "rm grep.txt grape.txt"
		p = subprocess.Popen(cmd, shell=True , stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		retval = p.wait()	


t = 0
for i in range(5):
	for j in range(5):
		testfile("test/files/"+str(i)+"/"+str(j))
	for j in range(5):
		for k in range(5):
			testfile("test/files/"+str(i)+"/"+str(j)+"/"+str(k))
		for k in range(5):
			for n in range(5):
				testfile("test/files/"+str(i)+"/"+str(j)+"/"+str(k)+"/"+str(n))
print(t)