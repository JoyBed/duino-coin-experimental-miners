import machine, usocket, uhashlib, ubinascii, sys, os # Only micropython included libraries
soc = usocket.socket()
username = "username here" # Edit this to your username, mind the quotes

# This section connects and logs user to the server
addr = usocket.getaddrinfo('51.15.127.80', 2811)[0][-1]
soc.connect(addr) # Connect to the server

# Mining section
while True:
  try:
      soc.send(bytes("JOB,"+str(username)+",ESP","utf8")) # Send job request
      job = soc.recv(1024).decode() # Get work from pool
      job = job.split(",") # Split received data to job (job and difficulty)
      difficulty = job[2]
  
      for result in range(100 * int(difficulty) + 1): # Calculate hash with difficulty
          ducos1 = uhashlib.sha1(str(job[0] + str(result))).digest() # Generate hash
          print(ducos1, job[0], job[1], job[2])
          ubinascii.hexlify(ducos1)
          b = bytes(ducos1, 'utf-8')
          if job[1] == b: # If result is even with job
              soc.send(bytes(str(result)+",,Minimal_PC_Miner","utf8")) # Send result of hashing algorithm to pool
              feedback = soc.recv(1024).decode() # Get feedback about the result
              if feedback == "GOOD": # If result was good
                  print("Accepted share", result, "Difficulty", difficulty)
                  break
              elif feedback == "BAD": # If result was bad
                  print("Rejected share", result, "Difficulty", difficulty)
                  break
  except Exception as err:
    print(err)
