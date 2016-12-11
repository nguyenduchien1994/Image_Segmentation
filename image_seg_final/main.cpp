//
//  main.cpp
//  Ford_Fulkerson
//
//  Created by Abhyudaya on 20/11/16.
//  Copyright © 2016 none. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <queue>
#include <unordered_map>
#include <map>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <chrono>

int V = 202;

class Graph {
public:
  //std::map<int,std::map<int,int> > adjacencyList;
   std::unordered_map<int,std::unordered_map<int,int> > adjacencyList;
    void addEdge(int source, int dest, int weight )
    {
        if ( adjacencyList.find(source) == adjacencyList.end() )
        {
            std::unordered_map<int,int> edge;
            edge.insert(std::pair<int,int>(dest,weight));
            adjacencyList.insert(std::pair<int, std::unordered_map<int,int> >(source,edge));
        }
        else
        {
            //std::map<int,int>& edgeList = adjacencyList.at(source);
            adjacencyList[source].insert(std::pair<int,int>(dest,weight));
        }
    }

  void numberOfEdges()
    {
        int size = 0;
        for ( auto& pair:adjacencyList )
        {
            size += pair.second.size();
        }
	std::cout<<"number of edges are "<<size<<"\n";
    }

    //void displa
    // We might need some functions but it's better we directly access from adjacencyList rather through function call;
};


int breadthFirstSearch(Graph& residualGraph, int source, int sink, std::vector<int>& parent, std::vector<bool>& reached, bool print)
{
    // We add one because our graph contains only adjacencyList;
    // May be we will change this in the future
    size_t V = residualGraph.adjacencyList.size();
    //std::vector<bool> reached(residualGraph.adjacencyList.size(),false);
    std::queue <int> queue;
    queue.push(source);
    reached[source] = true;
    parent[source] = -1;
    
    while (!queue.empty())
    {
        int u = queue.front();
        queue.pop();
        std::unordered_map<int, int>& neighbors = residualGraph.adjacencyList[u];
        //for (int v=0; v<V; v++)
        //{
        for(auto& edgeWeightPair:neighbors )
        {
            if (reached[edgeWeightPair.first]==false && edgeWeightPair.second > 0)
            {
                if (print)
                    std::cout << "neigbouring edge" << edgeWeightPair.first << std::endl;
                queue.push(edgeWeightPair.first);
                parent[edgeWeightPair.first] = u;
                reached[edgeWeightPair.first] = true;
            }
        }
    }
    if(reached[sink])
    {
        return true;
    }
    return false;
}


void depthFirstSearch(Graph& residualGraph, std::vector<bool>& reached, int source )
{
    //std::cout << "Setting " << source << "as true" << std::endl;
    reached[source] = true;
    for (int i = 0; i < V; ++i )
        if (residualGraph.adjacencyList[source][i] && !reached[i])
            depthFirstSearch(residualGraph,reached,i);
}


// method returns max flow using ford_fulkerson with breadth first search
cv::Mat Ford_Fulkerson (Graph& graph, int source, int sink, int width, int height )
{
    Graph* prevGraph = new Graph();
    prevGraph->adjacencyList = graph.adjacencyList;
    std::vector<bool> reached(graph.adjacencyList.size(),false);
    int u, v, maxFlow=0;
    //int parent[V];
    std::vector<int> parent(graph.adjacencyList.size());
    // check if there is a path from source to sink
    while (breadthFirstSearch(graph, source, sink, parent,reached, false))
    {
        // Find the directed edge with minimum capacity in the s-t path
        int minCapacityEdge = 10000000; // set some high value in the beginning;
        for (v=sink; v!=source; v=parent[v])
        {
            u = parent[v];
            minCapacityEdge = std::min(minCapacityEdge, graph.adjacencyList[u][v]);
        }
        // push flow equal to the minCapacityEdge in the s-t path
        // update the graph, increasing flow decreasing the capacity on forward direction
        // and increasing capacity on the revese directions
        for (v=sink; v != source; v=parent[v])
        {
            u = parent[v];
            graph.adjacencyList[u][v] -= minCapacityEdge;
            graph.adjacencyList[v][u] += minCapacityEdge;
        }
        
        // Update the flow with amount pushed.
        maxFlow += minCapacityEdge;
        std::fill(reached.begin(),reached.end(),false);
    }
    
    //breadthFirstSearch(graph, source, sink, parent,reached, true);
   
    /*for (int i = 0; i < prevGraph->adjacencyList.size(); i++)
    {
        for (int j = 0; j < prevGraph->adjacencyList.size(); j++)
        {
            if (reached[i] && !reached[j] && prevGraph->adjacencyList[i][j])
            {
                std::cout << i << " - " << j << std::endl;
            }
        }
	}*/
    
    int foregroundCount = 0;
    int backgroundCount = 0;
    cv::Mat binaryImage = cv::Mat(height,width,CV_8UC1);
    for (int i=0; i< prevGraph->adjacencyList.size()-1; ++i)
    {
        int rowVal = i/width;
        int colVal  = i%width;
        if ( reached[i+1] )
        {
            binaryImage.at<uchar>(rowVal,colVal) = 0;
            foregroundCount++;
        }
        else
        {
            binaryImage.at<uchar>(rowVal,colVal) = 255;
            backgroundCount++;
        }
    }
    std::cout << "foreground count is " << foregroundCount << std::endl;
    std::cout << "background count is " << backgroundCount << std::endl;

    //cv::imshow("Segmented Image",binaryImage);
    //cv::waitKey(0);
        
    
    //return maxFlow;
    return binaryImage;
}

Graph* createAdiGraph ( int width, int height, int penalty, std::vector<int>& likelihoodArray )
{
    int imageSize = width*height;
    std::cout << "The image size passed is " << imageSize << std::endl;
    Graph* g = new Graph();
    int threshold; //aditya
    int sum; //aditya
    for (int i=0; i<width*height; ++i )
    {
        g->addEdge(0,i+1,likelihoodArray.at(i));
        g->addEdge(i+1,(width*height)+1,10-likelihoodArray.at(i));
        sum = sum+likelihoodArray.at(i); //aditya
    }
    //threshold=10-(sum/width*height);
    threshold=9;
    for ( int i=1; i<= imageSize; ++i )
    {
        int rownum = (i-1)/width;
        int colnum = (i-1)%width;
        
        //The above rownum and colnum work when we start with 0,0 as our indices;
        //since here our vertices start with 1,1
        rownum = rownum+1;
        colnum = colnum+1;
        
        ////////////////////////////aditya////////////////////////////////////
        if(rownum != 1 && colnum != 1 && rownum != height && colnum != width){
            int weight1 = 10-abs(likelihoodArray.at(i)-likelihoodArray.at(i+1));
            int weight2 = 10-abs(likelihoodArray.at(i)-likelihoodArray.at(i-1));
            int weight3 = 10-abs(likelihoodArray.at(i)-likelihoodArray.at(i+width));
            int weight4 = 10-abs(likelihoodArray.at(i)-likelihoodArray.at(i-width));
            
            if(weight1>threshold)
                g->addEdge(i,i+1,penalty);
	    //else
	    //   g->addEdge(i,i+1,2);

            if(weight2>threshold)
                g->addEdge(i,i-1,penalty);
	    //else
	    //    g->addEdge(i,i-1,2);
	    
            if(weight3>threshold)
                g->addEdge(i,i+width,penalty);
	    //else
	    //    g->addEdge(i,i+width,2);
	    
            if(weight4>threshold)
                g->addEdge(i,i-width,penalty);
	    //else
	    //    g->addEdge(i,i-width,2);
		}
        ////////////////////////////////////////////////////////////////////
        // first row
        /*if ( rownum == 1 )
         {
         // horizontal next pixel
         if ( i != width )
         g->addEdge(i,i+1,penalty);
         
         // horizontal previous pixel
         if ( colnum != width )
         g->addEdge(i,i-1,penalty);
         
         // vertical below pixel
         g->addEdge(i,i+width,penalty);
         
         }
         else if ( colnum == 1 )  // first column
         {
         // we don't have to worry about the edge condition for pixel 1 since it will go to previous if loop;
         g->addEdge(i,i+1,penalty);
         g->addEdge(i,i-width,penalty);
         
         if ( rownum != height)             //last row;
         g->addEdge(i,i+width,penalty);
         
         }
         else if ( rownum == height )
         {
         if ( colnum != width )
         g->addEdge(i,i+1,penalty);
         
         g->addEdge(i,i-width,penalty);
         // no check required here since colnum 1 will go to previous loop
         g->addEdge(i,i-1, penalty);
         }
         else if ( colnum == width )
         {
         g->addEdge(i,i+width,penalty);
         g->addEdge(i,i-width,penalty);
         g->addEdge(i,i-1,penalty);
         }
         else
         {
         g->addEdge(i,i+width,penalty);
         g->addEdge(i,i-width,penalty);
         g->addEdge(i,i-1,penalty);
         g->addEdge(i,i+1,penalty);
         }*/
	 }
    
        g->numberOfEdges();
    return g;
}

Graph* createGraph ( int width, int height, int penalty, std::vector<int>& likelihoodArray )
{
    int imageSize = width*height;
    std::cout << "The image size passed is " << imageSize << std::endl;
    Graph* g = new Graph();
    for (int i=0; i<width*height; ++i )
    {
        g->addEdge(0,i+1,likelihoodArray.at(i));
        g->addEdge(i+1,(width*height)+1,10-likelihoodArray.at(i));
    }
    for ( int i=1; i<= imageSize; ++i )
    {
        int rownum = (i-1)/width;
        int colnum = (i-1)%width;
        
        //The above rownum and colnum work when we start with 0,0 as our indices;
        //since here our vertices start with 1,1
        rownum = rownum+1;
        colnum = colnum+1;
        // first row
        if ( rownum == 1 )
        {
            // horizontal next pixel
            if ( i != width )
                g->addEdge(i,i+1,penalty);
            
            // horizontal previous pixel
            if ( colnum != width )
                g->addEdge(i,i-1,penalty);
            
            // vertical below pixel
            g->addEdge(i,i+width,penalty);
            
        }
        else if ( colnum == 1 )  // first column
        {
            // we don't have to worry about the edge condition for pixel 1 since it will go to previous if loop;
            g->addEdge(i,i+1,penalty);
            g->addEdge(i,i-width,penalty);
            
            if ( rownum != height)             //last row;
                g->addEdge(i,i+width,penalty);
            
        }
        else if ( rownum == height )
        {
            if ( colnum != width )
                g->addEdge(i,i+1,penalty);
            
            g->addEdge(i,i-width,penalty);
            // no check required here since colnum 1 will go to previous loop
            g->addEdge(i,i-1, penalty);
        }
        else if ( colnum == width )
        {
            g->addEdge(i,i+width,penalty);
            g->addEdge(i,i-width,penalty);
            g->addEdge(i,i-1,penalty);
        }
        else
        {
            g->addEdge(i,i+width,penalty);
            g->addEdge(i,i-width,penalty);
            g->addEdge(i,i-1,penalty);
            g->addEdge(i,i+1,penalty);
        }
    }
    g->numberOfEdges();
    return g;
}

int main(int argc, const char * argv[]) {
    
    // insert code here...
    std::cout << "Ford Fulkerson implementatio begin() " << std::endl;
    std::string baselocation = ("/home/aditya/image_seg_final/");
    std::string imageFile = baselocation + std::string("plane.jpg");
    //std::string filename = baselocation + "likelihood.py";
    std::string filename = baselocation + "gmm_clustering.py";
      std::string command = "python ";
    command += filename;
    system(command.c_str());
    cv::Mat image;
    image = cv::imread(imageFile);
    
    if (!image.data)
    {
        std::cout << "Unable to read image file " << std::endl;
    }
    
    // read the kmeans output and store it as a vector
    std::string file = baselocation + std::string("gmmoutput.txt");
    std::ifstream label(file.c_str());
    std::vector<int> likelihoodArray;
    if ( label.is_open() )
    {
        std::string line;
        while (std::getline(label, line) )
        {
            likelihoodArray.push_back(std::stoi(line));
        }
        label.close();
    }
    else {
        std::cout << "Cannot read data file file" << std::endl;
    }
    
    std::cout << "The size of likelihood array is " << likelihoodArray.size() << std::endl;
    Graph* inputGraph = createGraph(image.cols,image.rows,4,likelihoodArray);
    inputGraph->addEdge(inputGraph->adjacencyList.size(),inputGraph->adjacencyList.size(),0);
    std::cout << "The size of the graph is " << inputGraph->adjacencyList.size() << std::endl;
    cv::Mat binaryImage= cv::Mat(image.cols,image.rows,CV_8UC1);
    auto start = std::chrono::system_clock::now();
    //std::cout << "The max flow value is " << Ford_Fulkerson(*inputGraph,0,inputGraph->adjacencyList.size()-1,image.cols, image.rows) << std::endl;
    binaryImage=Ford_Fulkerson(*inputGraph,0,inputGraph->adjacencyList.size()-1,image.cols, image.rows);
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout<<elapsed.count();
    std::cout<<"\n";

     cv::imshow("Segmented Image",binaryImage);
    cv::waitKey(0);
    return 0;
    
}
