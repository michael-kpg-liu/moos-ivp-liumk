Name: Z. Duguid & K. Raleigh
Date: May 11th 2019
File: README
-----------------------------------------------------------------------
This mission folder was designed for participation in the 2.680 Front
Estimation competition in May 2019. We pursued a two-part approach to 
our mission design:
(1). Scanning: first-pass survey of the region to gain an initial
     understanding of the thermal front.
(2). Spiraling: adaptively spiraling two vehicles in close proximity 
     to each other aligned with the most recent estimation of the
     thermal front axis.


Instructions for Running the `adaptive_spiral` mission:
(1). Specify a `captain` in the `plug_pSpiralPath.moos` file. Make 
     sure that the captain name provided corresponds with the name of 
     one of the vehicles being used. This parameter is used to assign
     control authority of the group to a particular vehicle in order to
     maintain coordination throughout the mission.
(2). Confirm the values for the `COOL_FAC`, `COOL_STEPS`, `CONCURRENT`, 
     and `ADAPTIVE` parameters specified in `launch_vehicle.sh` 
(2). Confirm that the `max_speed` variable in `plug_pEncircle.moos`
(2). Confirm the return time specified in `plug_uTimerScript.moos`
(3). Confirm the bounds specified in `plug_pFrontEstimateX.moos`
-----------------------------------------------------------------------
This mission folder has been designed to operate with the Heron
vehicles on the Charles River. The `alpha_huron` mission folder in the 
`moos-ivp/ivp/missions-2680` directory was used for integrating our
approach onto the Huron platform. The README instructions for the
`alpha_huron` mission are pasted below for reference.
-----------------------------------------------------------------------
Name: M.Benjamin
Date: May 10th 8th, 2019
File: README

This mission folder is meant to be a baseline bare-bones folder for
preparing to launch a simple Alpha home-plate mission on one of the
MIT Heron USVs. It can also be regarded as a template for creating
more specific mission folder, like the 2.680 Front Estimation mission
for example.

This mission is designed to support THREE modes of operation:

(1) Single machine simulation:
------------------------------
The "launch.sh" will support the launching of a simulation where both
the shoreside and all vehicle communities are running on a single
machine. For example, a single vehicle named "abe" may be launched
with time warp 10 with:

`$ ./launch.sh -a 10`

This will launch both the shoreside and vehicle MOOS communities.
More than one vehicle may be launched:

`$ ./launch.sh -a -b -c -d 10`

(2) Multi-machine simulation:
------------------------------
The "launch_shoreside.sh" and "launch_vehicle.sh" scripts will support
the launching of shore and vehicle communities on different machines.
This could mean multiple laptops, or more common in 2.680, a PABLO
running a vehicle simulation connected to another laptop running the
shoreside. For example:

On the shoreside machine:

`$ ./launch_shoreside.sh WARP`

Once this community is launched, there should be a pMarineViewer
window showing the shoreside IP address on the top bar. This should
be noted.

On the vehicle machine, e.g., PABLO:

`$ ./launch_vehicle.sh WARP --shoreip=192.168.2.1 -a --sim`

This will launch the vehicle named "abe", and looking for the shoreside
at the given IP address. The --sim flag launches the local vehicle
community launching uSimMarine (the vehicle simulator).

(3) Heron operation:
------------------------------
This mode is very similar to (2). As before launch the shoreside,
with no time warp, and note the IP address:

`$ ./launch_shoreside.sh WARP`

On the vehicle PABLO:

`$ ./launch_vehicle.sh --shoreip=192.168.2.1 -g`

This will launch the vehicle named "gus", and looking for the shoreside
at the given IP address. Notes: we don't specify a WARP since robot
ops are always warp=1. We don't invoke the --sim flag since this is not
a simulation. This will result in uSimMarine not launching and iM200
launching instead. iM200 is the PABLO interface to the Heron. Finally,
we launch with one of the actual robot names:

On Evan,  launch with -e
On Felix, launch with -f
On Gus,   launch with -g
On Hal,   launch with -h
On Ida,   launch with -i
On Jing,  launch with -j
On Kirk,  launch with -k
On Luke,  launch with -l

Note: the vehicle name is the name of the robot computer box, not
robot vehicle frame.
