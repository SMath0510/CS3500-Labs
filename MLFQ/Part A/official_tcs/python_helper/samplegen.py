for i in range(1, 6):
	file_name = "incom/incom" + (str)(i) + ".txt"
	f = open(file_name, "w")
	if(i <= 3):
		f.write("20 50000 inputs/" + (str)(i) + ".txt output/out" + (str)(i) + ".txt")
	if(i == 4):
		f.write("50 40 inputs/" + (str)(i) + ".txt output/out" + (str)(i) + ".txt")
	if(i == 5):
		f.write("20 100 inputs/" + (str)(i) + ".txt output/out" + (str)(i) + ".txt")
	
