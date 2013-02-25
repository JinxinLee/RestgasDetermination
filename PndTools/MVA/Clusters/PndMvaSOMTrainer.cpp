/* ****************************************
 * Self Organizing Map Training functions *
 * Author: M.Babai@rug.nl                 *
 * Version:                               *
 * LICENSE:                               *
 * ****************************************
 */
///////////////////////////////
#include <iostream>
#include <limits>
#include <cassert>
#include <cmath>
#include <algorithm>

#include "TRandom3.h"

#include "PndMvaSOMTrainer.h"
#include "PndSomNode.h"
#include "PndMvaUtil.h"
/////////////////////////////////
/**
 * Constructor.
 *@param InputData Input Data points.
 *@param mapWidth The width of the SOM.
 *@param mapHeight The height of the SOM.
 *@param numIter  Number of learning iterations.
 *@param initType The scheme to initialize weight of each node.
 */
PndMvaSomTrainer::PndMvaSomTrainer( DataPoints const* const InputData,
                                    size_t mapWidth, size_t mapHeight,
                                    size_t numIter,
                                    MapNodeInitType initType,
                                    GridInitType gridInitType)
  : m_sigmaZero(0.00),
    m_lambda(0.00),
    m_neighbourhoodRadius(0.00),
    m_MapWidth (mapWidth),
    m_MapHeight(mapHeight),
    m_NumModelVectors(m_MapWidth * mapHeight),
    m_NumIterations(numIter),
    m_InitMode(initType),
    m_GridType(gridInitType),
    m_TheMap(),
    m_DataSet(InputData)
{}

/**
 * Destructor.
 */
PndMvaSomTrainer::~PndMvaSomTrainer()
{
  for(size_t i = 0; i < m_TheMap.size(); ++i) {
    delete m_TheMap[i];
  }
  m_TheMap.clear();

  m_DataSet = 0;
}

/**
 * Initialize the map according to the given scheme.
 */
void PndMvaSomTrainer::InitMap()
{
  // Check if the data set is empty.
  assert( m_DataSet->size() != 0 );

  // Determine the start radius.
  m_sigmaZero = static_cast <double> (std::max(m_MapWidth, m_MapHeight) )/2.0;

  assert ( m_sigmaZero > 0);
  m_lambda    = static_cast <double> (m_NumIterations) / log(m_sigmaZero);

  // Init the map grid.
  switch(m_GridType) {
  case HEXAGONAL:// Hexagonal grid
    std::cout << "\t<-I-> Init Hexagonal map grid.\n";
    InitGridHexagonal();
    break;
  case RECTANGULAR:// Rectangular grid.
  default:
    std::cout << "\t<-I-> Init Rectangular map grid.\n";
    InitGridRectAngular();
  }

  // Init per node weight vector.
  switch(m_InitMode) {
  case SOM_RANDOM:// Random numbers
    std::cout << "\t<-I-> Init map nodes using Random numbers.\n";
    InitMapnodes_Random();
    break;
  case SOM_RAND_FROM_DATA:// Random vectors selected from data set
  default:
    std::cout << "\t<-I-> Init map nodes using Points from data set.\n";
    InitMapnodes_RandomFromData();
  }

  /*
   * Populate the neighbour list for all nodes in the map grid.
   */
  double distance;
  for(size_t n = 0; n < m_TheMap.size(); ++n) {
    // Current node
    PndSomNode* curr_node = m_TheMap[n];
    std::vector< std::pair<size_t, double> > neighbours;
    
    // Compute the distance to all other map nodes
    for(size_t p = 0; p < m_TheMap.size(); ++p) {
      PndSomNode* p_node = m_TheMap[p];
      // Distance = (dX)^2 + (dY^2)
      distance = (curr_node->GetXPos() - p_node->GetXPos()) *
                 (curr_node->GetXPos() - p_node->GetXPos()) +
                 (curr_node->GetYPos() - p_node->GetYPos()) *
                 (curr_node->GetYPos() - p_node->GetYPos());
      
      // Add to current neigbour list for the current node
      neighbours.push_back(std::make_pair(p, distance));
    }
    // The dist to all other nodes is determind. Add list
    curr_node->SetNeighbours(neighbours);
  }
}

/*
 * Init a rectangular grid.
 */
void PndMvaSomTrainer::InitGridRectAngular()
{
  for(size_t r = 0; r < m_MapHeight; ++r) {// rows
    for(size_t c = 0; c < m_MapWidth; ++c) {// columns
      // PndSomNode* node = new PndSomNode(left, top, right, bottom);
      PndSomNode* node = new PndSomNode(c, r, c+1, r+1);
      // Init node center
      node->InitNode();
      // Add the node
      m_TheMap.push_back(node);
    }
  }
  // Debug info
#if (PRINT_PND_SOMTRAIN_DEBUG_INFO > 0)
  printMapGrid();
#endif
}

/*
 * Init a hexagonal grid.
 */
void PndMvaSomTrainer::InitGridHexagonal()
{
  std::cerr << "\n\n<ERROR> NOT Implemented yet\n\n";
}

/*
 * Initialize map nodes using random vectors fetched from the data
 * set.
 */
void PndMvaSomTrainer::InitMapnodes_RandomFromData()
{
  // Fetch random examples from the data set.
  TRandom3 rnd;
  size_t index;
  
  for( size_t i = 0; i < m_NumModelVectors; ++i) {
    // Select a Random index
    index = static_cast<size_t>(rnd.Uniform(0, (m_DataSet->size() -1 )));
    // Fetch point
    std::vector<float> const* cur_example = (m_DataSet->at(index)).second;
    // Modify the node.
    PndSomNode *node = m_TheMap[i];
    node->SetWeight(*cur_example);
  }
#if (PRINT_PND_SOMTRAIN_DEBUG_INFO > 0)
  std::cout << " Number of model vectors is " << m_TheMap.size()
            << '\n';
#endif
}

/*
 * Initialize map nodes using vectors with random numbers. 
 */
void PndMvaSomTrainer::InitMapnodes_Random()
{
  // Non empty data set.
  assert(m_DataSet->size() != 0);

  // If the map nodes are initialized
  assert (m_TheMap.size() == m_NumModelVectors);

  // Find the vector length
  size_t dimension = ( (m_DataSet->at(0)).second)->size();
  TRandom3 rnd;

  // Map node loop.
  for( size_t i = 0; i < m_NumModelVectors; ++i) {
    std::vector<float> weight(dimension);
    // Dimension loop (init vector components).
    for(size_t d = 0; d < dimension; ++d) {
      weight[d] = static_cast<float>(rnd.Uniform());
    }
    // Modify the node.
    PndSomNode *node = m_TheMap[i];
    node->SetWeight(weight);
  }
#if (PRINT_PND_SOMTRAIN_DEBUG_INFO > 0)
  std::cout << " Number of model vectors is " << m_TheMap.size()
            << '\n';
#endif
}

/*
 *returns the index of the BMU map node.
 *
 *@param vector vector containing the coordinates of the current
 * data point.
 */
size_t PndMvaSomTrainer::FindBestMatchingNode(std::vector<float> const& vec)
{
  float min_dist = std::numeric_limits<float>::max();
  float cur_dist = 0.0;
  size_t mbu_index = 0;

  // Map node loop.
  for( size_t i = 0; i < m_NumModelVectors; ++i) {
    PndSomNode const* cur_node = m_TheMap[i];
    std::vector<float> const& cur_weights = cur_node->GetWeight();
    // Compute the distance to the current Map node.
    cur_dist = ComputeDist(cur_weights, vec);
    // Find the index of BMU
    if(cur_dist < min_dist) {
      min_dist = cur_dist;
      mbu_index = i;
    }
  }
  return mbu_index;
}

/**
 * Train map using batch schema. All available data vectors are
 * presented at once.
 */
void PndMvaSomTrainer::TrainBatch()
{
  std::cout << "<INFO> Training the map in batch mode.\n";

  // Non empty data set.
  assert( m_DataSet->size() != 0 );
  // Initialized map.
  assert ( m_TheMap.size() != 0 );
  // Dimension
  size_t dimension = (m_TheMap[0])->GetNodeDimension();

  /*
   * Expose the map to all available input (train data points).
   */
  size_t mapIndex;// BMU index
  size_t normCnt;// Number of neighbours
  double WidthSq;// Width of acceptance

  // Training loop (t_0 ... t_n);
  for(size_t t = 0; t < m_NumIterations; ++t) {
    // Reset responsibility lists for all nodes.
    for(size_t k = 0; k < m_TheMap.size(); ++k) {
      (*m_TheMap[k]).ResetRespList();
    }
    // Calculate the width of the neighbourhood for this step (time).
    assert(m_lambda > 0.00);
    m_neighbourhoodRadius = m_sigmaZero * exp( - static_cast<double>(t) / m_lambda);
    WidthSq = m_neighbourhoodRadius * m_neighbourhoodRadius;
    // Find BMU and and populate the responsibility lists.
    for( size_t i = 0; i < m_DataSet->size(); ++i) {
      std::vector<float> const* currInput = (m_DataSet->at(i)).second;
      mapIndex = FindBestMatchingNode( *currInput);
      // Add data point index to the node responsibility list.
      (m_TheMap[mapIndex])->AddToRespList(i);
    }
    /* Map node loop. Visit all nodes and update.*/
    for(size_t nd = 0; nd < m_TheMap.size(); ++nd) {
      normCnt = 0;
      // Current node
      PndSomNode* curr_node = m_TheMap[nd];
      std::vector<float> Cr_weight(dimension, 0.00);
      // Loop through the respo. list
      std::vector<size_t> const& resList = curr_node->GetRespoList();
      normCnt += resList.size();
      for(size_t ev = 0; ev < resList.size(); ++ev) {
        std::vector<float> const* evt = (m_DataSet->at(resList[ev])).second;
        // Update
        for( size_t d = 0; d < evt->size(); ++d) {
          Cr_weight[d] += evt->at(d);
        }
      }
      // List of the neighbors for the curren map node.
      std::vector< std::pair<size_t, double> > const& nls = curr_node->GetNeighbours();
      // Neighbour list loop.
      for(size_t b = 0; b < nls.size(); ++b) {
        // If the neighbor inside neighborhood dist.
        if(nls[b].second < WidthSq) {
          // The neighbor
          PndSomNode const* curr_neighb = m_TheMap[b];
          // The list of data points
          std::vector<size_t> const& rsl = curr_neighb->GetRespoList();
          normCnt += rsl.size();
          // Loop of data points
          for(size_t ev = 0; ev < rsl.size(); ++ev) {
            std::vector<float> const* dat = (m_DataSet->at(rsl[ev])).second;
            // Update
            for( size_t d = 0; d < dat->size(); ++d) {
              Cr_weight[d] += dat->at(d);
            }
          }
        }// If inside Ni
      }// All neighb.
      // Normalize current node
      if(normCnt != 0) {
        for( size_t d = 0; d < Cr_weight.size(); ++d) {
          Cr_weight[d] = Cr_weight[d] / static_cast<float>(normCnt);
        }
      }
      // Replace weight.
      curr_node->SetWeight(Cr_weight);

      // Print Debug info in debug mode
#if (PRINT_PND_SOMTRAIN_DEBUG_INFO > 2)
      std::cout << " size is "  << nls.size()
                << " wrSize = " << Cr_weight.size()
                << " and sqrt width = " << WidthSq
                << " neighCnt = " << normCnt
                << std::endl;
#endif
    }// Map nodes loop
  }// End Train loop
}

/**
 * Train the map using Online scheme. Data vectors are presented one
 * at a time.
 */
void PndMvaSomTrainer::TrainOnline()
{
  std::cerr << "<ERROR> Online training is not implemented yet.\n"
            << "\t Either you need to implement it or wait\n";
}

void PndMvaSomTrainer::Calibrate()
{
  std::cout << "<INFO> Calibrating the SOM based on majority counts.\n";
  
}

//_____________ DEBUG _________________
#if (PRINT_PND_SOMTRAIN_DEBUG_INFO > 0)
void PndMvaSomTrainer::printMapGrid() const
{
  for(size_t i = 0; i < m_TheMap.size(); ++i) {
    PndSomNode const* node = m_TheMap[i];
    std::cout << "<DEBUG> left = " << node->GetLeft()
              << " right = " << node->GetRight()
              << " top = " << node->GetTop()
              << " bottom = " << node->GetBottom()
              << " X-pos = " << node->GetXPos()
              << " Y-pos = " << node->GetYPos()
              << '\n';
  }
}
#endif
