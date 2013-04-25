#/usr/bin/env python
# -*- coding: utf-8 -*-

# Import required Python libraries
import MySQLdb
import serial
import time
import sys
import urllib2

# Funkce
def is_number(s):
  '''Determines if s is a number'''
  try:
    float(s)
    return True
  except ValueError:
    return False

def cleanup():
  '''Cleanup function'''
  print "DB Cleanup called..."
  
  # Truncate local database
  print "Truncating table..."
  cur.execute("TRUNCATE TABLE `speed`;")
  db.commit()
  
  # Try to truncate remote database
  try:
    url = "http://david.roesel.cz/tests/kuk/truncate.php"
    request = urllib2.Request(url)
    response = urllib2.urlopen(request)
    print "Truncating remote..."
  except:
    print "Remote truncate failed"

# Greet the user
print "Running background Arduino script for KUK (CTRL-C to exit)"

# Setting up the connection to the database
db = MySQLdb.connect(host="127.0.0.1", 
                     user="kuk", 
                     passwd="malina",
                     db="kuk")

# Creating cursor object
cur = db.cursor() 

# Trying to initiate serial COM
try:
  ser = serial.Serial('COM3', 9600, timeout=1)

except:
  cleanup()
  sys.exit("Failed to find Arduino.")
 
try:
  # Loop until user quits with CTRL-C
  
  # Look for arduinos reaction
  print ser.readline()
  time.sleep(1)
  
  # Send initial set of parameters
  params = "P230M0C"
  ser.write(params)
  time.sleep(1)
  # Wait for OK
  print "Reaction to P: "+ser.readline()
  time.sleep(1)
  
  # Start accelerating
  ser.write('A')
  time.sleep(1)
  # Wait for OK
  print "Reaction to A: "+ser.readline()
  time.sleep(1)
  
  # Time to throw the ball
  print("------- Ready to roll! --------")
  
  # Interval to wait / 2
  halfint = 0.2
  zapis = ""
    
  while True :
    # Look for new parameters
    f=open("settings.txt")
    newparams = f.readlines()[0]
    f.close()
    
    # Ask for current speed measurement
    ser.write('V')
    time.sleep(halfint)
    # Try to fetch it
    try: 
      speed = ser.readline()
    except: 
      print("Failed to fetch speed!")
    
    # Print speed
    print("Speed: "+str(speed))
    
    # Looking for new parameters
    if newparams != params:
      params=newparams
      ser.write(str(params))
      print("Succesfully set new function setting: "+str(params))
      print ser.readline()
    time.sleep(halfint)      
    
    # If we have finite speed
    if (is_number(speed) and str(speed)!="inf\r\n"):
      # Edit it to just float
      speed = speed.replace("\r\n", "")
      
      # Insert it into the database
      cur.execute("INSERT INTO `speed` (`speed`) VALUES ("+str(speed)+");")
      db.commit() # commit the Insert
      
      # Append dump data
      zapis += str(speed)+"\n"
      
      # Try to insert speed into remote database
      try:
        url = "http://david.roesel.cz/tests/kuk/input.php?speed="+speed
        request = urllib2.Request(url)
        response = urllib2.urlopen(request)
      except:
        print("Remote data insert failed")
 
except KeyboardInterrupt:
  print "\nReceived Ctrl+C..."

  # Stop Arduino
  print "Stopping Arduino..."
  ser.write('S')
  
  # Call for cleanup
  cleanup()
  
  # Dump the speeds into file
  g=open("./dumpy/"+str(time.time())+".txt", "w") # Otevírám soubor do kterého budu zapisovat
  g.write(zapis) # Zapisuji a zavírám
  g.close()
  
  # Exit
  sys.exit("All done! I hope it went well.")
