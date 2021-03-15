import os, machine, usocket, uhashlib, ubinascii, time # Only micropython included libraries
soc = usocket.socket()
username = "username here" # Edit this to your username, mind the quotes

# This section connects and logs user to the server
addr = usocket.getaddrinfo('51.15.127.80', 2811)[0][-1] # Initialise the server address to WiFi buffer
soc.connect(addr) # Connect to the server
server_version = soc.recv(3).decode() # Get server version
print("Server is on version", server_version) # Print server version

# Mining section
try:
  while True:
      soc.send(bytes("JOB,"+str(username)+",ESP","utf8")) # Send job request
      job = soc.recv(1024).decode() # Get work from pool
      job = job.split(",") # Split received data to job (job and difficulty)
      difficulty = job[2]
      start = time.time() 
      for result in range(100 * int(difficulty) + 1): # Calculate hash with difficulty
          ducos = str(ubinascii.hexlify(uhashlib.sha1(str(job[0] + str(result))).digest())) # Generate hash and convert it to string
          if job[1] == ducos[2:42]: # If result is even with job
              end = time.time()
              hastime  = end - start
              hashrate = result / hastime
              soc.send(bytes(str(result), "utf8")) # Send result of hashing algorithm to pool
              feedback = soc.recv(1024).decode() # Get feedback about the result
              if feedback == "GOOD": # If result was good
                  print("Accepted share", result, "Difficulty", difficulty, "Hashrate", hashrate, "H/s", "Hashtime", hastime)
                  break
              elif feedback == "BAD": # If result was bad
                  print("Rejected share", result, "Difficulty", difficulty)
                  break
                
except Exception as err:
    print(err)
