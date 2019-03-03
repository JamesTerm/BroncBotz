# BroncBotz
BroncBotz source code
The goal of this repo is to be a one-stop archive to any source that can be pulled for at anytime for reference, and to be used in another development repository

This is still a work in progress including the document, but currently the structure is as so:
Branches:
PortLibs - initial port of libraries
PortRobots - initial port of robots

I may merge these into master, but which ever... these initial have no history, because the history can be found from its origin.

One thing that is for certain is that we can merge repositories each year, and after testing with git subtree features, it seems the safest to do a simple merge of the working repository of that year into this one.  This repository an archive repository which is designed as a reference to easily pull what students need for the following year, so its commit bandwidth should be low.

One thing I am not certain about yet is the idea of working with the git subtree feature.  I may try this with Curivator robot since it spans over all the years.  I may make a working repository of it that makes use of the subtree.  It is possible for other libraries to work in this regard as well.  We'll have to see if it is a good idea.  For students they can work without subtrees by simply copying what files they want to use into a newly created repository for that year.  It doesn't matter that the history before that point is not copied because it can be traced back from the annual repo merge.

For now, the branches will remain until Ryan has ported some of the c# years into this.  After which, we should probably merge into master since this is an archive only repository.

