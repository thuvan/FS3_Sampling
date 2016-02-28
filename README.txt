This code use MCMC sampling to return a set of sample subgraphs using uniform distribution.
The code is not that optimized and should not be used if you are looking to find all frequent
subgraphs. 

If you use this code for any of your work, please cite the following article:

"Output Space Sampling for Graph Patterns, In Proc. of the 35th International Conference on Very 
Large Data Bases (VLDB-2009), Lyon, France, August, 2009"

TO COMPILE:

step1 : make clean
step2 : make

TO RUN:

./uniform_sampling.out -d ./dataset/GRAPH_large.dat -c 1000 -s 30

-d  = input file  (see the dataset directory for input graph format)
-c  = maximum iteration
-s  = minimum support

for any question contact:

Mohammad Al Hasan (Author)
Email: alhasan@cs.iupui.edu


!!!!!!Enjoy!!!!!!!
