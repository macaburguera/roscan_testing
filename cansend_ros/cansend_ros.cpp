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


class cansendclass{
public:
	float numFloat;
	int numInt;
	void callback1(const std_msgs::Float32::ConstPtr& msg){
		numFloat = msg->data;
		ROS_INFO_STREAM("float message received: "<<msg->data);
		ROS_INFO_STREAM("float message passed in callback: "<<numFloat);

	}


	void callback2(const std_msgs::Int8::ConstPtr& msg){
		numInt = msg->data;
		ROS_INFO_STREAM("int message received: "<<msg->data);
		ROS_INFO_STREAM("int message passed in callback: "<<numInt);
	}

};

int main(int argc, char **argv){
	ros::init(argc,argv, "cansend");
	ros::NodeHandle nh;



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

	while(ros::ok()){

		cansendclass obj1;
		
		//create subscriber for topic 1
		ros::Subscriber sub1 = nh.subscribe("topic1_can", 1000, &cansendclass::callback1, &obj1);

		//create publisher for topic 2
		ros::Subscriber sub2 = nh.subscribe("topic2_can", 1000, &cansendclass::callback2, &obj1);

		sleep(2);

		frame.can_id = 0x500;
		frame.can_dlc = 8;

		ROS_INFO_STREAM("integer is "<<obj1.numInt);

		frame.data[0] = obj1.numFloat*10;
		frame.data[1] = obj1.numInt;
		frame.data[2] = obj1.numInt+10;
		frame.data[3] = obj1.numInt;
		frame.data[4] = obj1.numInt-4;
		frame.data[5] = obj1.numInt;
		frame.data[6] = obj1.numInt+100;
		frame.data[7] = obj1.numInt;


		nbytes = write(s, &frame, sizeof(struct can_frame));
		ros::spinOnce();

	}
} 