#!/usr/bin/python

# Import modules for CGI handling 
from os import environ
import fileinput
# import cgi, cgitb

def print_error():
	print("Content-Type: text/html\r")
	print("Status: 600 Invalid data\r\n\r")
	print("<html><head><title>Invalid data</title>\r")
	print("<h1>Invalid data typed</h1>\r")

def print_text(output):
	print("Content-Type: text/html\r")
	print("Status: 200 ok\r\n\r")
	print("<html><head><title>ok</title>\r")
	print("<h1>ouput : %s</h1>\r" % output)	

def handlePOST():
	input_data = ""
	for line in fileinput.input():
		input_data += line
	print_text(input_data)

def handleGET():
	if environ.has_key('QUERY_STRING'):
		query_string = environ['QUERY_STRING']
		print_text(query_string)
	else:
		print_error()

if environ.has_key('REQUEST_METHOD'):
	request_method = environ['REQUEST_METHOD']
	if (request_method == 'POST'):
		handlePOST()
	elif (request_method == 'GET'):
		handleGET()
	else:
		print_error()
else:
	print_error()
