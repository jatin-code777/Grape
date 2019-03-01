#         Grape

[![Build Status](https://travis-ci.com/IITH-SBJoshi/concurrency-7.svg?token=kgRmNj9ALBoMQM8mWL6x&branch=master)](https://travis-ci.com/IITH-SBJoshi/concurrency-7)

Multithreaded Alternative to grep

grep which is the most popular tool used to make regex searches on Linux systems, has been around for a while. The current implementation of grep is single-threaded and does not make use of modern multi-core systems. This is specially evident when the search is spread over a large number of files and directories.
Thus, we plan to implement an improved version of grep which makes use of modern concurrency and parallelism.

# instructions to install 
clone the repository:	<pre>git clone "https://github.com/IITH-SBJoshi/concurrency-7.git"
</pre>

create directory for build
<pre>
  cd concurrency-7 
  mkdir build && cd build
</pre>

build the project
<pre>
  cmake .. && make 
</pre>
sudo required to execute from any position.
<pre>
  sudo cp ./grape /bin/
</pre>


# Usage
<pre>
grape [OPTION]... PATTERN [FILE]
Search for PATTERN in each FILE.
Example: grape -i 'hello world' main.c

Pattern selection and interpretation:
 -F,   --fixed-string         {default}PATTERN is a normal string
 -G,   --basic-regexp         PATTERN is a basic regular expression (default)
 -e,   --regexp=PATTERN       use PATTERN for matching
 -i,   --ignore-case          ignore case distinctions

Miscellaneous:
       --version              display version information and exit
       --help                 display this help text and exit
       --usage                display usage text and exit

Output control:
 -n,   --line-number          print line number with output lines
 -r,   --recursive            recursive search inside directory
 -r,   --recursive            recursive search inside directory
 -r,   --recursive            recursive search inside directory
 -L,   --files-without-match  print only names of FILEs with no selected lines
 -l,   --files-with-matches   print only names of FILEs with selected lines


</pre>	
