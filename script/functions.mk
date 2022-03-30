f_subdir = $(filter-out $(1), $(shell ls -l| grep ^d | awk '{print $$9}')) 


export f_subdir