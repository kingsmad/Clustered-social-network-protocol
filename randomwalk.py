import numpy as np
import random
import time

class Directions:
	n = (0, 1)
	s = (0, -1)
	e = (1, 0)
	w = (-1, 0)
	ne = (1, 1)
	se = (1, -1)
	nw = (-1, 1)
	sw = (-1, -1)

def tuple_add(t1, t2):
	return (t1[0] + t2[0], t1[1] + t2[1])
	 
def outrange(t):
	x = t[0]
	y = t[1]
	return x < 0 or x >= 1000 or y < 0 or y >= 1000

def randomewalk():
	position = []

	direction = [Directions.n, Directions.s, Directions.e, Directions.w,
					Directions.ne, Directions.nw, Directions.se, Directions.sw]

	s = set()
	#start point
	start = (random.randrange(0, 1000),random.randrange(0, 1000))
	s.add(start)
	position.append(start)

	#end point
	end = start
	while end in s:
		end = (random.randrange(0, 1000),random.randrange(0, 1000))

	print(start)
	print(end)

	#distance = (end[0]-start[0], end[1]-start[1])
	distance = (start[0]-end[0], start[1]-end[1])

	#walk
	step_prev = (0,0)
	while distance != (0,0):	
			#                                               0      1     2    3    4    5    6     7
		if distance[0] > 0 and distance[1] > 0:
			step_cur = direction[np.random.choice(8, 1, p=[0.05, 0.15, 0.05, 0.15, 0.0, 0.05, 0.05, 0.5])[0]]
		elif distance[0] < 0 and distance[1] > 0:
			step_cur = direction[np.random.choice(8, 1, p=[0.15, 0.05, 0.05, 0.15, 0.05, 0.0, 0.5, 0.05])[0]]
		elif distance[0] > 0 and distance[1] < 0:
			step_cur = direction[np.random.choice(8, 1, p=[0.05, 0.15, 0.15, 0.05, 0.05, 0.5, 0.0, 0.05])[0]]
		elif distance[0] < 0 and distance[1] < 0:
			step_cur = direction[np.random.choice(8, 1, p=[0.15, 0.05, 0.15, 0.05, 0.5, 0.05, 0.05, 0.0])[0]]
		elif distance[0] == 0:
			step_cur = direction[np.random.choice(8, 1, p=[0.35, 0.35, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05])[0]]
		elif distance[1] == 0:
			step_cur = direction[np.random.choice(8, 1, p=[0.05, 0.05, 0.35, 0.35, 0.05, 0.05, 0.05, 0.05])[0]]

		if tuple_add(step_prev, step_cur) != (0,0):
			distance = tuple_add(distance, step_cur)
			start = tuple_add(start, step_cur)
			position.append(start)
			step_prev = step_cur

	return position

def main():
	node = 200
	with open("nodes.txt", "w") as f:
		print(node, file=f, end='\n')
		for i in range(node):
			output = ""
			pos = randomewalk()
			output += "{} ".format(str(len(pos)))
			for each in pos:
				pair = "{} {} ".format(each[0], each[1])
				output += pair
			print(output, file=f, end='\n')

if __name__ == '__main__':
	main()
