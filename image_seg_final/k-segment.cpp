#include <iostream>
#include <limits.h>
#include <string.h>
#include <queue>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;
//supposing we have a k prob matrix that returns an array of k prob values for each pixel 
//assuming the prob values are between 0 and 1
float kprob(int k, int image[V][W][3])
{
	return kprob[V][W][k];
}
//supposing we have ford fulkerson as such it takes in a set of pixels in graph and returns
//the labels of the pixels as 1 or -1
int FFS2(int graph[V][W], int s, int t)
{
    return pixellabel[V][W];
}

//needs to take in image I'm not sure how you guys did this but assuming I somehow get 
//V*W pixel image with 3 colors need to take in V and W also as image size each time we 
//input the prob matrix we have to normalize or else we have to take 2 prob values since 
//after the first segmentation the sink probs are no longer 1-prob(A)
//make all other prob of the pixel=0 once the pixel is segmented into a group
int main()
{
	int j, i, k;
	image image[V][W][3]
	cout << "input number of segments" << endl;
	cin >> k;
	int prob[V][W][k];
	int currentsegmentprob[V][W];
	int column, row;
	int s,t;
	int segmentmatrix[V][W];
	int temp[V][W];
	prob=kprob(k, image);
	// take the first prob here
	for (row=1; row<V; row++)
		for (column=1; column<W; column++)
			currentsegmentprob[row][column]=prob[row][column][1];
	//impiment s and t in correct fashions
	//first segmentation
	segmentmatrix=FFS2(currentsegmentprob, s, t);
	for (row=1; row<V; row++)
		for (column=1; column<V; column++)
		{
			if(segmentmatrix[row][column]==1)
			{
				prob[row][column][1]=1;
				for(i=2; i<k+1; i++)
					prob[row][column][i]=0;
			}
			else
			{
				prob[row][column][1]=0;
			}
		}
	
	for (i=2; i<k; i++)
	{	
	//normalization
		int sum[V][W]={0};
		for (row=1; row<V; row++)
			for (column=1; column<W; column++)
				for (j=i; j<k; j++)
					sum[row][column]=sum[row][column]+prob[row][column][j];
		for (row=1; row<V; row++)
			for (column=1; column<W; column++)
				for (j=i; j<k; j++){
					if (prob[row][column]==0)
						currentsegmenprob[row][column]=0;
					else
						currensegmentprob[row][column]=prob[row][column][i]/sum[row][column]
					}
		//fordfulkerson here again
		temp=FFS2(currentsegmentprob, s, t);
		for (row=1; row<V; row++)
			for (column=1; column<V; column++)
			{	
			//deletion of nodes from prob matrix (assigning zero prob to all other values)
				if(temp[row][column]==1)
				{	
					prob[row][column][i]=1;
					segmentmatrix[row][column]=i;
					For(j=i+1; j<k+1; j++)
						prob[row][column][j]=0;
				}
				else
				{
					prob[row][column][i]=0;
				}
			}				
	}
	
	return 0;
}



