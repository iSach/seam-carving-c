# Seam Carving

This project was done as a class project for the following class:
Data Structures and Algorithms (INFO0902-1)

It is a C implementation of a seam carving algorithm, with dynamic programming.

The goal is to resize images smartly to avoid squeezing the important parts of the image. We do that by detecting the important parts of the image, and only deleting the others.

The brute-force search method requires an exponential execution time. By using
programming, it is reduced to a linear execution time.

Grade:
* Code: `20/20`
* Report: `18/20`
* Overall: `19/20`

### Running

First, compile:  
`gcc -o seamcarving mainSlimming.c slimming.c PNM.c`

Then, run:  
`./seamcarving original.pnm carved.pnm k`  
Where k is the number of pixels to remove.

### Report
The release folder contains a pdf report, answering some theoretical questions
about the project. (In French)

### Assignment
[Click here for the assignment pdf](http://www.montefiore.ulg.ac.be/~jmbegon/2019_2020/sda/p3.pdf)

### Example

Original image:


![original](https://i.imgur.com/n7vSK6c.png "Original")

After resizing:


![resized](https://i.imgur.com/eGmH2yh.png "Resized")
