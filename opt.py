import numpy as np 
import cvxopt as cvxopt
from cvxpy import *
from cvxpy import Variable, power
from cvxopt.modeling import variable
from cvxopt.modeling import op
import socket 
import sys
import signal


def signal_handler(signal, frame):
    print('You pressed Ctrl+C!')
    sys.exit(0)

def convex_solv(P_set,P1_set,P2_set,alpha,beta,gamma):
	# alpha = 1
	# beta = 1
	# gamma = 1

	Blender_current = 1.8
	Heater_current = 2.3

	Blender_tol = 0.5*120 *Blender_current
	Heater_tol = 0.6*120 *Heater_current


	#optimization variables
	P_1 = Variable()
	P_2 = Variable()
	g  = power(P_1-P1_set,2)
	h = power(P_2-P2_set,2)
	l = power(P_1+P_2-P_set,2)

	#constraints 
	constraints = [P_1 >= Blender_tol,P_1 <= 120*Blender_current,P_2 <= 120*Heater_current,P_2 >= Heater_tol]

	#Form Objective
	obj = Minimize(alpha*l+beta*g+gamma*h)

	#solving problem
	prob = Problem(obj,constraints)
	prob.solve()
	# print(prob.status)
	print("P1 Value: ", P_1.value)
	print("P2 Value: " , P_2.value)
	return P_1.value, P_2.value

def tcp_init():
	serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# hostname = socket.gethostbyname(socket.gethostname());
	# print ( hostname );
	serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	serversocket.bind(('192.168.2.5', 10000))
	#serversocket.bind(("localhost",100))
	serversocket.listen(5) # become a server socket, maximum 5 connections
	connection, address = serversocket.accept()
	print("Incoming "+str(address))
	return connection;
			

def tcp(p1,p2,con):	    	
	# h = str(p1)
	# l = str(p2)
	message = "%6.3f,%6.3f,\0" % (p1,p2);
	refresh_cnt = 0
	while True:		
		buf = con.recv(64)
		if len(buf) > 0:
			con.sendall(message)
			refresh_cnt = refresh_cnt +1;
			# print('received data',buf)

		if refresh_cnt > 5:
			break
	    

	# connection.close()
	# serversocket.close()

	# TCP_IP = 'localhost'
	# TCP_PORT = 5005 
	# BUFFER_SIZE = 20
	# s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
	# s.bind((TCP_IP,TCP_PORT))
	# s.listen(1)
	# (conn,addr) = s.accept()
	# print ('Connection address:',addr)
	# while 1: 
	# 	data = conn.recv(BUFFER_SIZE)
	# 	if not data: break
	# 	print ("received data:",data)
	# 	conn.send(data)
	# conn.close()
def main():
	signal.signal(signal.SIGINT, signal_handler) # register ctrl c interrupt
	P_set = 0
	P2_set = 0
	P1_set = 0
	alpha = input("How much do you want to follow power grid set point (scale 1-10)?: ")
	alpha = int(alpha)
	beta = input("How much do you want to follow the Blender's set point (scale 1-10)?: ")
	beta = int(beta)
	gamma = input ("How much do you want to follow the Heater's set point (scale 1-10)?: ")
	gamma = int(gamma)
	print(' ')

	isConnect = 0;
	while(1):
		P_set = input("total power set point : ")
		P_set = int(P_set)
		P1_set = input("Blender power set point: ")
		P1_set = int(P1_set)
		P2_set = input("Heater power set point : ")
		P2_set = int(P2_set)
		p1,p2 = convex_solv(P_set,P1_set,P2_set,alpha,beta,gamma)
		print(' ')

		if (not isConnect):
			con = tcp_init()
			isConnect = 1

		tcp(p1,p2,con)
	
if __name__ == "__main__":
    main()