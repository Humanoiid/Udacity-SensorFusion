/* \author Aaron Brown */
// Create simple 3d highway enviroment using PCL
// for exploring self-driving car sensors

#include "sensors/lidar.h"
#include "render/render.h"
#include "processPointClouds.h"
// using templates for processPointClouds so also include .cpp to help linker
#include "processPointClouds.cpp"
#include "quiz/cluster/cluster.h"
#include "quiz/cluster/cluster.cpp"

std::vector<Car> initHighway(bool renderScene, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    Car egoCar( Vect3(0,0,0), Vect3(4,2,2), Color(0,1,0), "egoCar");
    Car car1( Vect3(15,0,0), Vect3(4,2,2), Color(0,0,1), "car1");
    Car car2( Vect3(8,-4,0), Vect3(4,2,2), Color(0,0,1), "car2");	
    Car car3( Vect3(-12,4,0), Vect3(4,2,2), Color(0,0,1), "car3");
  
    std::vector<Car> cars;
    cars.push_back(egoCar);
    cars.push_back(car1);
    cars.push_back(car2);
    cars.push_back(car3);

    if(renderScene)
    {
        renderHighway(viewer);
        egoCar.render(viewer);
        car1.render(viewer);
        car2.render(viewer);
        car3.render(viewer);
    }

    return cars;
}


void simpleHighway(pcl::visualization::PCLVisualizer::Ptr& viewer)
{
    // ----------------------------------------------------
    // -----Open 3D viewer and display simple highway -----
    // ----------------------------------------------------
    
    // RENDER OPTIONS
    // bool renderScene = true;
    bool renderScene = false;
    bool render_obst = true;
    bool render_plane = true;
    bool render_clusters = true;
    bool render_box = true;


    std::vector<Car> cars = initHighway(renderScene, viewer);
    Lidar* lidar = new Lidar(cars,0);

    // TODO:: Create lidar sensor 
    pcl::PointCloud<pcl::PointXYZ>::Ptr inputCloud = lidar->scan();
    // renderRays(viewer,lidar->position,inputCloud);
    // renderPointCloud(viewer,inputCloud,"inputCloud");
    // TODO:: Create point processor
    ProcessPointClouds<pcl::PointXYZ>* pointProcessor = new ProcessPointClouds<pcl::PointXYZ>(); 

    std::pair<typename pcl::PointCloud<pcl::PointXYZ>::Ptr, typename pcl::PointCloud<pcl::PointXYZ>::Ptr> segmentCloud = pointProcessor-> SegmentPlane(inputCloud, 100, 0.2);
    if(render_obst)
        renderPointCloud(viewer, segmentCloud.first, "obstacle",Color(1,0,0));
    if(render_plane)
        renderPointCloud(viewer, segmentCloud.second, "planeCloud",Color(0,1,0)); 

    std::vector<pcl::PointCloud<pcl::PointXYZ>::Ptr> cloudClusters = pointProcessor->Clustering(segmentCloud.first, 1.0, 3, 30);
    int clusterId = 0;
    std::vector<Color> colors = {Color(1,0,0), Color(1,1,0), Color(0,0,1)};

    for(pcl::PointCloud<pcl::PointXYZ>::Ptr cluster : cloudClusters)
    {
        if(render_clusters)
        {
            std::cout<<"cluster size";
            pointProcessor->numPoints(cluster);
            renderPointCloud(viewer,cluster,"obstCloud"+std::to_string(clusterId),colors[clusterId]);
        }
        if(render_box)
        {
            Box box = pointProcessor->BoundingBox(cluster);
            renderBox(viewer,box,clusterId);
        }
        
        ++clusterId;
    }
}


//setAngle: SWITCH CAMERA ANGLE {XY, TopDown, Side, FPS}
void initCamera(CameraAngle setAngle, pcl::visualization::PCLVisualizer::Ptr& viewer)
{

    viewer->setBackgroundColor (0, 0, 0);
    
    // set camera position and angle
    viewer->initCameraParameters();
    // distance away in meters
    int distance = 16;
    
    switch(setAngle)
    {
        case XY : viewer->setCameraPosition(-distance, -distance, distance, 1, 1, 0); break;
        case TopDown : viewer->setCameraPosition(0, 0, distance, 1, 0, 1); break;
        case Side : viewer->setCameraPosition(0, -distance, 0, 0, 0, 1); break;
        case FPS : viewer->setCameraPosition(-10, 0, 0, 0, 0, 1);
    }

    if(setAngle!=FPS)
        viewer->addCoordinateSystem (1.0);
}


void cityBlock(pcl::visualization::PCLVisualizer::Ptr& viewer)
{
    //simple rendering example 
    // open 3D viewer and display City Block
    ProcessPointClouds<pcl::PointXYZI>* pointProcessorI = new ProcessPointClouds<pcl::PointXYZI>();
    pcl::PointCloud<pcl::PointXYZI>::Ptr inputCloud = pointProcessorI->loadPcd("../src/sensors/data/pcd/data_1/0000000000.pcd");
    // renderPointCloud(viewer,inputCloud,"InputCloud");
    
    pcl::PointCloud<pcl::PointXYZI>::Ptr filterCloud;
    
    filterCloud = pointProcessorI->FilterCloud(inputCloud, 3, Eigen::Vector4f(-10, -5, -2, -1), Eigen::Vector4f(30, 8, 1, 1));
    renderPointCloud(viewer,filterCloud,"filterCloud");
    
}
// const and & are used for constant reference for memory efficiency with the variable not change ------ no.... inputCloud here show error with Const....
void cityBlock_stream(pcl::visualization::PCLVisualizer::Ptr& viewer, ProcessPointClouds<pcl::PointXYZI>* pointProcessor, pcl::PointCloud<pcl::PointXYZI>::Ptr& inputCloud)
{
    
    inputCloud = pointProcessor->FilterCloud(inputCloud, 0.3, Eigen::Vector4f (-10, -5, -2, 1), Eigen::Vector4f (30, 8, 1, 1));
    
    // // "segment plane" based on PCL RANSAC or Own RANSAC
    // std::pair<pcl::PointCloud<pcl::PointXYZI>::Ptr, pcl::PointCloud<pcl::PointXYZI>::Ptr> segmentCloud = pointProcessor->SegmentPlane(inputCloud, 25, 0.3);
    std::pair<pcl::PointCloud<pcl::PointXYZI>::Ptr, pcl::PointCloud<pcl::PointXYZI>::Ptr> segmentCloud = pointProcessor->SegmentPlane_OwnRANSAC(inputCloud, 25, 0.3);

    // renderPointCloud(viewer, segmentCloud.first, "obstacle",Color(1,0,0));
    renderPointCloud(viewer, segmentCloud.second, "planeCloud",Color(0,1,0)); 


    // // "clustering object" based on PCL RANSAC or Own RANSAC
    // std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudClusters = pointProcessor->Clustering(segmentCloud.first, 0.53, 10, 500);
    // std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudClusters = pointProcessor->Clustering_Own(segmentCloud.first, 0.53, 10, 500);
        std::vector<pcl::PointCloud<pcl::PointXYZI>::Ptr> cloudClusters = pointProcessor->Clustering_Own(segmentCloud.first, 0.53, 15, 450);

    int clusterId = 0;
    std::vector<Color> colors = {Color(1,0,0), Color(1,1,0), Color(0,0,1)};

    for(pcl::PointCloud<pcl::PointXYZI>::Ptr cluster : cloudClusters)
    {
        std::cout << "cluster size ";
        pointProcessor->numPoints(cluster);
        renderPointCloud(viewer, cluster, "obstCloud"+std::to_string(clusterId),colors[clusterId%colors.size()]);

        Box box = pointProcessor->BoundingBox(cluster);
        renderBox(viewer, box, clusterId);
        ++clusterId;
    }

}


int main (int argc, char** argv)
{
    std::cout << "starting enviroment" << std::endl;

    pcl::visualization::PCLVisualizer::Ptr viewer (new pcl::visualization::PCLVisualizer ("3D Viewer"));
    CameraAngle setAngle = FPS;
    initCamera(setAngle, viewer);


    // // [PCD single Frame]
    // // ===============================================

    // // enable one function
    // simpleHighway(viewer);
    // cityBlock(viewer);

    // while (!viewer->wasStopped ())
    // {
    //     viewer->spinOnce ();
    // } 
    // // ===============================================



    // // [Stream the PCD data]
    // // ===============================================
    ProcessPointClouds<pcl::PointXYZI>* pointProcessorI = new ProcessPointClouds<pcl::PointXYZI>();
    // // load data
    // // * stream data: "../src/sensors/data/pcd/data_1"
    // // * stream data for challenge (bicycle) "../src/sensors/data/pcd/data_2"
    std::vector<boost::filesystem::path> stream = pointProcessorI->streamPcd("../src/sensors/data/pcd/data_1");
    auto streamIterator = stream.begin();
    pcl::PointCloud<pcl::PointXYZI>::Ptr inputCloudI;
    // // ===============================================

    while (!viewer->wasStopped ())
    {

        // // uncomment when Stream PCD data
        // // ====
        // Clear viewer
        viewer->removeAllPointClouds();
        viewer->removeAllShapes();

        // Load pcd and run obstacle detection process
        inputCloudI = pointProcessorI->loadPcd((*streamIterator).string());
        cityBlock_stream(viewer, pointProcessorI, inputCloudI);

        streamIterator++;
        if(streamIterator == stream.end())
            streamIterator = stream.begin();
        // // ====
        viewer->spinOnce ();
    } 
}