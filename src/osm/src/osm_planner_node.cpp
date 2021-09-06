/*
 * test.cpp
 *
 *  Created on: 17.10.2016
 *      Author: michal
 */
#include "rclcpp/rclcpp.hpp"
#include <osm_planner/osm_planner.h>
#include <osm_planner/osm_localization.h>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <mymsgs/srv/new_target.hpp>
#include <iostream>
#include <string>
#include <osm_planner/CoorConv.hpp>
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.h"

rclcpp::Node::SharedPtr n; 
using namespace std;
//extern std::shared_ptr<Localization>  localization_source_;
osm_planner::Planner plan;
osm_planner::Parser parser;
//std::shared_ptr<osm_planner::Parser> map = std::make_shared<osm_planner::Parser>(); 
   
//osm_planner::Localization localization(map, "source");
 
    bool makePlanCallback(const std::shared_ptr<mymsgs::srv::NewTarget::Request> req,std::shared_ptr<mymsgs::srv::NewTarget::Response> res) 
    {
    	RCLCPP_INFO(n->get_logger(),"plan");
        //boost::shared_ptr<const nav_msgs::Odometry> odom = ros::topic::waitForMessage<nav_msgs::Odometry>(odom_topic_, ros::Duration(3));
        res->result = plan.makePlan(req->latitude, req->longitude);
        return true;
    }

    void gpsCallback(const sensor_msgs::msg::NavSatFix::SharedPtr msg) 
    {
    	RCLCPP_INFO(n->get_logger(),"gps_init");
    	plan.setPosition(msg->latitude,msg->longitude,msg->altitude,true);
        //localization.setPositionFromGPS(msg->latitude,msg->longitude,msg->altitude,true);
    }

    void utmCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg) 
    {
    	RCLCPP_INFO(n->get_logger(),"utm_init");
    	double x = msg->pose.position.x;
    	double y = msg->pose.position.y;
    	int zone = int(msg->pose.position.z);
    	double lat;
    	double lon;
    	double roll, pitch, yaw;//定义存储r\p\y的容器
    	geometry_msgs::msg::Quaternion orientation;
    	orientation = msg->pose.orientation;
    	tf2::Quaternion quat(orientation.x,orientation.y,orientation.w,orientation.z);

    	tf2::Matrix3x3 m(quat);
    	m.getRPY(roll, pitch, yaw);//进行转换

    	WGS84Corr latlon;
    	UTMXYToLatLon (x,y,zone,false,latlon);
    	lat = RadToDeg(latlon.lat);
    	lon = RadToDeg(latlon.log);
    	plan.setPosition(lat,lon,yaw,true);
        //localization.setPositionFromGPS(msg->latitude,msg->longitude,msg->altitude,true);
    }



int main(int argc, char **argv) {

    rclcpp::init(argc, argv);
    n = rclcpp::Node::make_shared("test_osm") ;
    
	
    n->declare_parameter("osm_map_path");
    n->declare_parameter("filter_of_ways");
    n->declare_parameter("origin_latitude");
    n->declare_parameter("origin_longitude");
    n->declare_parameter("interpolation_max_distance");
    
    
    double origin_lat, origin_lon;
    n->get_parameter("origin_latitude",origin_lat);
    n->get_parameter("origin_longitude",origin_lon);
            
            
    
    std::string file = "map.osm";
    n->get_parameter("osm_map_path",file);
    
    
    
    std::vector<std::string> types_of_ways ;
    types_of_ways.push_back("footway"); 
    types_of_ways.push_back("secondary"); 	
    n->get_parameter("filter_of_ways",types_of_ways);
    

   
    double interpolation_max_distance = 2.0;
    n->get_parameter("interpolation_max_distance",interpolation_max_distance);
    
    
    
    plan.initialize(n, origin_lat, origin_lon, file);
    

    
    rclcpp::Subscription<sensor_msgs::msg::NavSatFix>::SharedPtr gps_sub;
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr utm_sub;
	
    gps_sub = n->create_subscription<sensor_msgs::msg::NavSatFix>("gps",1,&gpsCallback);
    utm_sub = n->create_subscription<geometry_msgs::msg::PoseStamped>("init_utm",1,&utmCallback);
    rclcpp::Service<mymsgs::srv::NewTarget>::SharedPtr service =n->create_service<mymsgs::srv::NewTarget>("make_plan", &makePlanCallback);
  
    while (rclcpp::ok()) 
    {
    
        rclcpp::spin_some(n);
        
    }
    return 0;
}


