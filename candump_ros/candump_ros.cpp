#include <ros/ros.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Int8.h>

#include <stdint.h>
#include <stdio.h>

#include <string.h>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <net/if.h> //provides data structures to manage network interfaces
#include <sys/types.h> //https://pubs.opengroup.org/onlinepubs/007908775/xsh/systypes.h.html
#include <sys/socket.h>

#include <sys/ioctl.h> 
#include <linux/can.h>
#include <linux/can/raw.h>

int main(int argc, char **argv){
	ros::init(argc,argv, "candump");
	ros::NodeHandle nh;

	//create publisher for topic 1
	ros::Publisher pub1 = nh.advertise<std_msgs::Float32>("topic1_can", 1000);
	//create publisher for topic 2
	ros::Publisher pub2 = nh.advertise<std_msgs::Int8>("topic2_can", 1000);


	int s;
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	struct ifreq ifr;
	strcpy(ifr.ifr_name, "vcan0"); //vcan0 is the interface name
	ioctl(s, SIOCGIFINDEX, &ifr);

	struct sockaddr_can addr;
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(s, (struct sockaddr*)&addr, sizeof(addr));

	int nbytes;
	struct can_frame frame;
	float f = 0;
	std_msgs::Float32 msgfloat;
	std_msgs::Int8 msgint;

	while(ros::ok()){
		nbytes = read(s, &frame, sizeof(struct can_frame));

		__u8 b[] = {frame.data[3], frame.data[2], frame.data[1], frame.data[0]};
		memcpy(&f, &b, sizeof(f));
		msgfloat.data = f;
		msgint.data = frame.data[5];

		pub1.publish(msgfloat);
		pub2.publish(msgint);

	}
} 