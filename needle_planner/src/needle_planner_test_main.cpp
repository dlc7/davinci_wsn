/* 
 * File:   needle_planner_test_main.cpp
 * Author: wsn
 *
 * Created Feb 24, 2016
 */
#include <needle_planner/needle_planner.h>
#include <davinci_kinematics/davinci_kinematics.h>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <geometry_msgs/Polygon.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Int32MultiArray.h"

//example use of needle-planner library
int main(int argc, char** argv) 
{
    // ROS set-ups:
    ros::init(argc, argv, "needle_planner_test_main"); //node name

    ros::NodeHandle nh; // create a node handle; need to pass this to the class constructor
    Eigen::Vector3d entrance_pt,exit_pt,tissue_normal;
    tissue_normal<<0,0,-1; //antiparallel to optical axis
    entrance_pt<<0.0,0.1,0.1; //100mm under camera; slightly forward, to avoid jnt lims should be OK
    exit_pt<<0.01,0.1,0.1; // exit pt is shifted along camera-frame +x axis relative to entrance pt

    vector <Eigen::Affine3d> gripper_affines_wrt_camera;//put answers here 
    vector <Eigen::Affine3d> gripper2_affines_wrt_camera;
    
    gripper_affines_wrt_camera.clear();
    gripper2_affines_wrt_camera.clear();

    Eigen::VectorXi ik_ok_array(40); //Add by DLC 5-26-2016 also in test_main, test_main_v2
    int ik_score=0;

    ROS_INFO("main: instantiating an object of type NeedlePlanner");
    NeedlePlanner needlePlanner;  
    //compute the tissue frame in camera coords, based on point-cloud selections:
    needlePlanner.compute_tissue_frame_wrt_camera(entrance_pt,exit_pt, tissue_normal);
    //optional: manually set the needle grasp pose, else accept default from constructor
    //needlePlanner.set_affine_needle_frame_wrt_gripper_frame(affine);
    //optional: set the initial needle frame w/rt tissue frame (or accept default in constructor)
    //needlePlanner.set_affine_needle_frame_wrt_tissue(Eigen::Affine3d affine)
    // given grasp transform, tissue transform and initial needle pose w/rt tissue,
    // compute needle-drive path as rotation about needle-z axis:


    needlePlanner.compute_needle_drive_gripper_affines(gripper_affines_wrt_camera, gripper2_affines_wrt_camera, ik_ok_array, ik_score);
    int nposes = gripper_affines_wrt_camera.size();
    ROS_INFO("computed %d gripper poses w/rt camera",nposes);
    Eigen::Affine3d affine_pose;
    for (int i=0;i<nposes;i++) {
        ROS_INFO("pose %d",i);
        cout<<gripper_affines_wrt_camera[i].linear()<<endl;
        cout<<"origin: "<<gripper_affines_wrt_camera[i].translation().transpose()<<endl;
    }
    needlePlanner.write_needle_drive_affines_to_file(gripper_affines_wrt_camera);
    
    return 0;
} 
