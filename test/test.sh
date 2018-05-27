#!/bin/bash
for i in {1..17};do
	./../parser $i.c >> result
done
