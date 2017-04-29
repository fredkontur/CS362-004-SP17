When I first tried to implement the random tester, I set it up as follows:
- Used rand() % maxValue to choose a random char
- Had inputChar() return a random char between 0-255
- Had inputString() return a string of 5 random chars between 0-255
I considered having the length of the string be random as well, but I thought
that would cause the program to run too long.

I found that the above conditions did not find a solution after 1/2 hour of 
running time. Therefore, I made the following change:
- Limit all random char choices to the standard character set, 32-126

I again found that the program could not find a solution after 1/2 hour of 
running. I then made some calculations. I determined that the odds of putting
together a particular string of 5 characters from among the 95 character 
choices I was allowing was 1 in 95^5 = 1 in 7.8 billion. It was taking about 5
minutes to go through 10 million iterations, so I would have to run the program
for about 60 hours to find the solution with these conditions. I then 
considered having the string being chosen from only the letters of the 
alphabet (upper- and lower-case). However, this would still lead to a predicted
running time of 3 hours. Finally, I determined how long the program would have
to run if the string was only chosen from the lower-case letters. In this case,
the program would have to run for 6 minutes, which was more reasonable. 
Therefore, I made the following change:
- For inputString(), limit the random char choices to only the lower-case 
  letters a-z

With this change, I was able to find the error message in 7 minutes after
15 million iterations. Finally, I was uncomfortable with the fact that my 
inputString() function was using malloc() to create the string without freeing 
the memory later. To avoid the memory leak this would create, I changed the 
string creation in inputString to use a static char rather than malloc. After 
doing this, I tested the program again and was able to find the error message
in 6 minutes after 12 million itertions.
