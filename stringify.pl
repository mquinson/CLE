#! /usr/bin/perl

use strict;

while (<>) {
    chomp;
    s/\\/\\\\/g;
    s/"/\\"/g;
    s/^/    "/; # ";
    s/$/\\n"\n/; #";
    print;
}