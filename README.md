# networking-Congestion-Control-

<h3>We will investigate the differences between Congestion Control ("CC") algorithms, specifically, between "cubic", which is the depolytic, and "reno" </h3>

We will need a large file so we will create one. Attached to the task is a file named create_large_file.py, run it and it will create a file 

Both files: sender.c and measure.c, The first will send our file and the second will receive it and measure how long it took to receive it all. Then, we will make a change in their CC algorithm and repeat the submission and calculation. 
To simulate packet loss, we will use a Linux tool called tc. 

*To download this tool: <h5>sudo apt install iproute</h5> 


We will then create a random loss of facts:

<h5> sudo tc qdisc add dev lo root netem loss 10% </h5> 

We will want to change the percentage of fact loss in each sample round:

<h5> sudo tc qdisc change dev lo root netem loss XX% </h5> 

When we want to measure 10,15,20,25,30 percent loss.

To return to the normal mode we will use the command: 

<h5> sudo tc qdisc del dev lo root netem </h5> 


<h5>sender.c</h5>
This program sends a file through a socket.
How the program works:

    * 1. Open Socket TCP.
    
    * 2. Create a connection with measure
    
    * 3. Send the file five times.
    
    * 4. Modification of CC algorithm.
    
    * 5. Submit the file five times.
    
    * 6. Close connection. 
    
     
     
   <h5>  measure.c </h5>
This program receives the file sent from the sender program via Socket.
How the program works:
   
   * 1. Open Socket TCP.
   
   * 2. Listening to incoming connections.
   
   * 3. Receive a connection from the sender.
   
   * 4. Receive the file five times while measuring the receipt time
   
   * 5. Average performance for sampled times.
   
   * 6. Modification of CC algorithm.
   
   * 7. Receive the file five times while measuring receipt time
   
   * 8. Average performance for sampled times.
   
   * 9. Print the measured times.
    
   * 10. Close connection. 
