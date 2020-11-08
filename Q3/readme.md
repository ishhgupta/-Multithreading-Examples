#Question 3
#Music Mayhem

##Overview
##Implementation
THree structs are declared which are passed during the inialization of thread functions.
INformation is stored in either global variables or arrays of the considered structs.

##code is divided in 3 parts

-onstage
-getting reward
-performer_came


###onstage
-All the performers that are to play get in the forever runnign while loop
-When a performance starts the status of stage is changed.
-The appropriate satge is given according to the type of musician.

-The stage the resets after the performance.


###getreward
-In this function  each performer after performing gets a reward.
-This goes if coordinators are free.

###stages
-IN this Each thread  sleeps for the given amount of time and then prints the message about arrival.
-Leaving bass and voilin all musicians can go either on acoustic or electric stages .
-The appropriate satge is given according to the type of musician.
-IF the time passes the impatience time the musician leaves.this is implemented by if cond.
-After performance the status of performer changes accordingly.


----Singers are considered same as other musicians ie it is not implemented.
--givin