import _thread, usocket, uhashlib, os, time, ubinascii

threadnumber = 2 #Thread number
username = "username"

def mining(i, username):
    addr = usocket.getaddrinfo('51.15.127.80', 2811)[0][-1]
    soc = usocket.socket()
    soc.connect(addr)
    soc.recv(3).decode()
    while True:
        try:
            soc.send(bytes("JOB,"+str(username)+",ESP","utf8")) # Send job request
            job = soc.recv(1024).decode() # Get work from pool
            job = job.split(",") # Split received data to job (job and difficulty)
            difficulty = job[2]
            start = time.time()
            for result in range(100 * int(difficulty) + 1): # Calculate hash with difficulty
                ducos1 = str(ubinascii.hexlify(uhashlib.sha1(str(job[0] + str(result))).digest()))
                if job[1] == ducos1[2:42]:
                    end = time.time()
                    hastime  = end - start
                    hashrate = result / hastime
                    soc.send(bytes(str(result) + "," + ",Multithread ESP32 MicroPython", "utf8"))
                    feedback = soc.recv(1024).decode()
                    if feedback == "GOOD": # If result was good
                        print("Thread:", i, "Accepted share", result, "Difficulty", difficulty, "Hashrate", hashrate, "H/s", "Hashtime", hastime)
                        break
                    elif feedback == "BAD": # If result was bad
                        print("Thread:", i, "Rejected share", result, "Difficulty", difficulty)
                        break
        except Exception as err:
            print(err)

print("ESP32 MicroPython Multithreaded Miner")
for i in range(threadnumber):
    _thread.start_new_thread(mining, (i, username))
    print("Starting thread:", i)
