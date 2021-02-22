import usocket, uhashlib # Only micropython included libraries
soc = usocket.socket()

username = "JoyBed" # Edit this to your username, mind the quotes

# This section connects and logs user to the server
soc.connect((str(51.15.127.80), int(2811))) # Connect to the server

# Mining section
while True:
    soc.send(bytes("JOB,"+str(username)+",ESP", encoding="utf8")) # Send job request
    job = soc.recv(1024).decode() # Get work from pool
    job = job.split(",") # Split received data to job (job and difficulty)
    difficulty = job[2]

    for result in range(100 * int(difficulty) + 1): # Calculate hash with difficulty
        ducos1 = uhashlib.sha1(str(job[0] + str(result)).encode("utf-8")).hexdigest() # Generate hash
        if job[1] == ducos1: # If result is even with job
            soc.send(bytes(str(result)+",,Minimal_PC_Miner", encoding="utf8")) # Send result of hashing algorithm to pool
            feedback = soc.recv(1024).decode() # Get feedback about the result
            if feedback == "GOOD": # If result was good
                print("Accepted share", result, "Difficulty", difficulty)
                break
            elif feedback == "BAD": # If result was bad
                print("Rejected share", result, "Difficulty", difficulty)
                break
